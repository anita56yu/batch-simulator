#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "job_storage.h"

#define MAX_JOBS 100001 // Define a maximum number of jobs for heap capacity

// Helper function to remove a job from all data structures
static void remove_job_from_all_structures(struct JobStorage* job_storage, struct Job* job);

// Comparison function for SJF (Min-Heap on job time)
int compare_sjf(const void* a, const void* b) {
    return ((struct Job*)a)->time - ((struct Job*)b)->time;
}

// Comparison function for Priority (Max-Heap on job priority)
int compare_priority(const void* a, const void* b) {
    return ((struct Job*)b)->priority - ((struct Job*)a)->priority;
}

// Index setting functions for the heaps
void set_sjf_index(void* data, int index) {
    ((struct Job*)data)->sjf_heap_index = index;
}

void set_priority_index(void* data, int index) {
    ((struct Job*)data)->priority_heap_index = index;
}

//initializing a job stoage
void setup_job_storage(struct JobStorage* job_storage){
   job_storage->queue_head = NULL;
   job_storage->running_job = NULL;
   job_storage->policy=0;
   job_storage->count=0;
   job_storage->total_submitted=0;
   job_storage->raw_start_time=time(NULL);
   job_storage->raw_turnaround_time=0;
   job_storage->raw_waiting_time=0;
   job_storage->raw_cpu_time=0;
   job_storage->raw_expected_waiting_time=0;
   job_storage->sjf_heap = heap_create(MAX_JOBS, compare_sjf, set_sjf_index);
   job_storage->priority_heap = heap_create(MAX_JOBS, compare_priority, set_priority_index);
}

//initializing a job
void setup_job(struct Job* job, char const* name, time_t arrival, int time, int priority, bool is_benchmark){
   strcpy(job->name, name);
   job->next_job=NULL;
   job->prev_job=NULL; // Initialize prev_job
   job->raw_arrival_time=arrival;
   job->time=time;
   job->priority=priority;
   job->is_benchmark=is_benchmark;
   job->sjf_heap_index = -1;
   job->priority_heap_index = -1;
}

//add a job to the queue in job_stoage
//If the queue was empty return true, otherwise return false
bool add_to_queue(struct JobStorage* job_storage, struct Job* job){
   bool was_empty = job_storage->queue_head == NULL;
   struct Job* current = job_storage->queue_head;
   struct Job* prev = NULL;

   // FCFS doubly linked list insertion
   while (current != NULL && difftime((current->raw_arrival_time), (job->raw_arrival_time)) < 0) {
       prev = current;
       current = current->next_job;
   }

   if (prev == NULL) { // Insert at head
       job->next_job = job_storage->queue_head;
       if (job_storage->queue_head != NULL) {
           job_storage->queue_head->prev_job = job;
       }
       job_storage->queue_head = job;
   } else { // Insert in middle or at tail
       job->next_job = current;
       job->prev_job = prev;
       prev->next_job = job;
       if (current != NULL) {
           current->prev_job = job;
       }
   }

   // Insert into SJF Min-Heap
   heap_insert(job_storage->sjf_heap, job);

   // Insert into Priority Max-Heap
   heap_insert(job_storage->priority_heap, job);

   job_storage->count++;
   job_storage->total_submitted++;
   job_storage->raw_expected_waiting_time+=job->time;
   return was_empty;
}

//reschedule the queue in job_storage according to new policy
void reschedule(struct JobStorage* job_storage, int policy){
   job_storage->policy = policy;
   // All complex list-rebuilding logic is removed.
   // The heaps and FCFS list are maintained by add_to_queue and pop_queue.
}

//pop the queue in job_storage and returns it
struct Job* pop_queue(struct JobStorage* job_storage){
   struct Job* job_to_run = NULL;

   switch (job_storage->policy) {
       case 0: // FCFS
           job_to_run = job_storage->queue_head;
           break;
       case 1: // SJF
           job_to_run = (struct Job*)heap_extract_top(job_storage->sjf_heap);
           break;
       case 2: // Priority
           job_to_run = (struct Job*)heap_extract_top(job_storage->priority_heap);
           break;
   }

   if (job_to_run != NULL) {
       remove_job_from_all_structures(job_storage, job_to_run);
       job_storage->count--;
       job_to_run->raw_start_time = time(NULL);
       job_storage->raw_waiting_time += job_to_run->raw_start_time - job_to_run->raw_arrival_time;
   }
   job_storage->running_job = job_to_run;
   return job_to_run;
}

//updates measurements of performance evaluation when a job is done
void job_done(struct JobStorage* job_storage, struct Job* job){
   job_storage->running_job=NULL;
   job_storage->raw_expected_waiting_time-=job->time;
   job_storage->raw_cpu_time+=time(NULL)-job->raw_start_time;
   job_storage->raw_turnaround_time+=time(NULL)-job->raw_arrival_time;
}

// Helper function to remove a job from all data structures
static void remove_job_from_all_structures(struct JobStorage* job_storage, struct Job* job) {
    // Remove from FCFS doubly linked list
    if (job->prev_job) {
        job->prev_job->next_job = job->next_job;
    } else { // Job is the head of the list
        job_storage->queue_head = job->next_job;
    }
    if (job->next_job) {
        job->next_job->prev_job = job->prev_job;
    }

    // Remove from SJF heap (if not already removed by heap_extract_top)
    if (job->sjf_heap_index != -1) {
        heap_remove_at_index(job_storage->sjf_heap, job->sjf_heap_index);
    }

    // Remove from Priority heap (if not already removed by heap_extract_top)
    if (job->priority_heap_index != -1) {
        heap_remove_at_index(job_storage->priority_heap, job->priority_heap_index);
    }
}
