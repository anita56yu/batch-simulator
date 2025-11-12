#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "job_storage.h"
#include "execute.h"
#include "user_interface.h"


#define NUM_THREADS 2
#define TCOUNT 30

struct JobStorage * ptr_job_storage;
int num_benchmark_job;

pthread_mutex_t benchmark_mutex;
pthread_cond_t benchmark_done_cv;
pthread_mutex_t queue_mutex;
pthread_cond_t queue_empty_cv;

//Takes input from interface and schedule jobs into job_storage according to requirement
void *scheduler(void *t)
{
    long my_id = (long)t;
    print_main_menu();
    while (true) {
        struct Input input;
        struct Input* ptr_input = &input;
        interface(ptr_input);
        if(ptr_input->input_type==RUN){
            pthread_mutex_lock(&queue_mutex);
            time_t rawtime = time ( NULL );
            struct Job* ptr_tmp = malloc(sizeof(struct Job));
            setup_job(ptr_tmp, ptr_input->job_name, rawtime, ptr_input->time, ptr_input->priority, false);
            int was_not_empty = add_to_queue(ptr_job_storage, ptr_tmp);
            print_submit_job(ptr_job_storage, ptr_tmp);
            if(!was_not_empty){
                pthread_cond_signal(&queue_empty_cv);
            }
            pthread_mutex_unlock(&queue_mutex);
        }
        else if(ptr_input->input_type==CHANGE_POLICY){
            pthread_mutex_lock(&queue_mutex);
            reschedule(ptr_job_storage, ptr_input->policy);
            print_reschedule(ptr_job_storage);
            pthread_mutex_unlock(&queue_mutex);
        }
        else if(ptr_input->input_type==LIST){
            pthread_mutex_lock(&queue_mutex);
            list_jobs(ptr_job_storage);
            pthread_mutex_unlock(&queue_mutex);
        }
        else if(ptr_input->input_type==TEST){
            struct Benchmark* ptr_benchmark = ptr_input->benchmark;
            pthread_mutex_lock(&benchmark_mutex);
            num_benchmark_job=ptr_benchmark->num_of_jobs;
            pthread_mutex_unlock(&benchmark_mutex);
            pthread_mutex_lock(&queue_mutex);
            setup_job_storage(ptr_job_storage);
            reschedule(ptr_job_storage, ptr_benchmark->policy);
            pthread_mutex_unlock(&queue_mutex);
            int i=0;
            for(; i<ptr_benchmark->num_of_jobs; i++){
                struct Job* ptr_tmp=malloc(sizeof(struct Job));
                generate_job(ptr_benchmark, ptr_tmp);
                pthread_mutex_lock(&queue_mutex);
                int was_not_empty = add_to_queue(ptr_job_storage, ptr_tmp);
                if(!was_not_empty){
                    pthread_cond_signal(&queue_empty_cv);
                }
                pthread_mutex_unlock(&queue_mutex);
                run_for_time(ptr_benchmark->arrival_rate);
            }
            free(ptr_benchmark);
            pthread_mutex_lock(&benchmark_mutex);
            if(num_benchmark_job>0){
                pthread_cond_wait(&benchmark_done_cv, &benchmark_mutex);
            }
            pthread_mutex_unlock(&benchmark_mutex);
            pthread_mutex_lock(&queue_mutex);
            quit_listing(ptr_job_storage);
            pthread_mutex_unlock(&queue_mutex);
        }
        else if(ptr_input->input_type==QUIT){
            break;
        }
        else{
            continue;
        }
    }
    pthread_exit(NULL);
}

//takes jobs from job_storage and dispatch them to executer
void *dispatcher(void *t)
{
    long my_id = (long)t;
    while (true) {
        pthread_mutex_lock(&queue_mutex);
        struct Job* job = pop_queue(ptr_job_storage);
        if (job == NULL) {
            pthread_cond_wait(&queue_empty_cv, &queue_mutex);
            job = pop_queue(ptr_job_storage);
        }
        pthread_mutex_unlock(&queue_mutex);
        if(job->is_benchmark){
            execute_benchmark(job);
            pthread_mutex_lock(&benchmark_mutex);
            num_benchmark_job--;
            if(num_benchmark_job<=0){
                pthread_cond_signal(&benchmark_done_cv);
                pthread_mutex_unlock(&benchmark_mutex);
            }
            else{
                pthread_mutex_unlock(&benchmark_mutex);
            }
        }
        else
            execute_job(job);
        pthread_mutex_lock(&queue_mutex);
        job_done(ptr_job_storage, job);
        pthread_mutex_unlock(&queue_mutex);
        free(job);
    }
    pthread_exit(NULL);
}

//This is the main function
int main(int argc, char *argv[])
{
    int i, rc;
    long t1=1, t2=2;
    pthread_t threads[2];
    pthread_attr_t attr;
    ptr_job_storage = malloc(sizeof(struct JobStorage));
    setup_job_storage(ptr_job_storage);
/* Initialize mutex and condition variable objects */
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init (&queue_empty_cv, NULL);
    pthread_mutex_init(&benchmark_mutex, NULL);
    pthread_cond_init (&benchmark_done_cv, NULL);
/* For portability, explicitly create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], &attr, scheduler, (void *)t1);
    pthread_create(&threads[1], &attr, dispatcher, (void *)t2);
/* Wait for scheduler to complete */
    pthread_join(threads[0], NULL);
    pthread_cancel(threads[1]);
    printf ("Main(): Waited and joined with %d threads.\n", NUM_THREADS);
/* Clean up and exit */
    quit_listing(ptr_job_storage);
    free(ptr_job_storage);
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_empty_cv);
    pthread_mutex_destroy(&benchmark_mutex);
    pthread_cond_destroy(&benchmark_done_cv);
    pthread_exit (NULL);
}
