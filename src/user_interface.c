#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "evaluate.h"
#include "user_interface.h"

char* policy_names[3]={"FCFS", "SJF", "PRIORITY"};

//prints out the initial main menu
void print_main_menu(){
    printf("Welcome to Wan Syun's batch job scheduler Version 2.0\nType 'help' to find more about AUbatch commands.\n");
}

//gets input from command line
void get_input(char * command){
    scanf("%s", command);
}

//prints help message
void print_help(){
    printf("run <job> <time> <pri>: submit a job named <job>,\n\t\t\texecution time is <time>,\n\t\t\tpriority is <pri>.\n");
    printf("list: display the job status.\n");
    printf("fcfs: change the scheduling policy to FCFS.\n");
    printf("sjf: change the scheduling policy to SJF.\n");
    printf("priority: change the scheduling policy to priority.\n");
    printf("test <benchmark> <policy> <num_of_jobs> <priority_levels>\n");
    printf("     <arrival_rate> <min_CPU_time> <max_CPU_time>\n");
    printf("quit: exit AUbatch\n");
}

//initializing struct input
void setup_input(struct Input* ptr_input, char const* command){
    strcpy(ptr_input->command, command);
    ptr_input->input_type=ERROR;
    ptr_input->policy=0;
    ptr_input->priority=0;
    ptr_input->time=0;
}

//lists all jobs in the queue
void list_jobs(struct JobStorage* job_storage){
    printf("Total number of jobs in the queue: %d\n", job_storage->count);
    printf("Scheduling Policy: %s\n", policy_names[job_storage->policy]);
    printf("%-10s %-10s %-10s %-15s %-10s\n", "Name", "CPU_Time", "Pri", "Arrival_time", "Progress");
    struct Job* current = job_storage -> queue_head;
    if(job_storage->running_job!=NULL){
        struct Job running = *(job_storage->running_job);
        struct tm * timeinfo = localtime ( &(running.raw_arrival_time) );
        char buffer[10];
        strftime(buffer, 10, "%X", timeinfo);
        printf("%-10s %-10d %-10d %-15s %-10s\n", running.name, running.time, running.priority, buffer, "Run");
    }
    while(current!=NULL){
        struct tm * timeinfo = localtime ( &(current->raw_arrival_time) );
        char buffer[10];
        strftime(buffer, 10, "%X", timeinfo);
        printf("%-10s %-10d %-10d %-15s\n", current->name, current->time, current->priority, buffer);
        current = current->next_job;
    }
}

//lists performance measurement (is used when quitting)
void quit_listing(struct JobStorage* job_storage){
    double num_job_executed = (job_storage->total_submitted-job_storage->count);
    printf("Total number of job submitted: %d\n", job_storage->total_submitted);
    printf("Average turnaround time: %.2lf seconds\n", ((double)job_storage->raw_turnaround_time)/num_job_executed);
    printf("Average CPU time:        %.2lf seconds\n", ((double)job_storage->raw_cpu_time)/num_job_executed);
    printf("Average waiting time:    %.2lf seconds\n", job_storage->raw_waiting_time/num_job_executed);
    printf("Throughput:              %.3lf No./second\n", num_job_executed/(time(NULL)-job_storage->raw_start_time));
}

//prints reschedule message
void print_reschedule(struct JobStorage* job_storage){
    printf("Scheduling policy is switched to %s. All the %d waiting jobs have been rescheduled.\n", policy_names[job_storage->policy], job_storage->count);
}

//prints job information 
void print_submit_job(struct JobStorage* job_storage, struct Job* job){
    printf("Job %s was submitted.\n", job->name);
    printf("Total number of jobs in the queue: %d\n", job_storage->count);
    printf("Expected waiting time: %d seconds\n", job_storage->raw_expected_waiting_time-job->time);
    printf("Scheduling Policy: %s.\n", policy_names[job_storage->policy]);
}

//transform policy char array into int
int get_policy(char const * policy){
    if(strcmp(policy, "fcfs")==0){ 
        return 0;
    }
    else if(strcmp(policy, "sjf")==0){
        return 1;

    }
    else if(strcmp(policy, "priority")==0){
        return 2;
    }
    return -1;
}

//prints out the menu and get command line command 
//returns a input to caller
void interface(struct Input* ptr_input){
    char command[100];
    get_input(command);
    setup_input(ptr_input, command);
    if (strcmp(command, "help")==0){
        print_help();
        ptr_input->input_type=HELP;
    }
    else if(strcmp(command, "list")==0){
        ptr_input->input_type=LIST;
    }
    else if(strcmp(command, "quit")==0){
        ptr_input->input_type=QUIT;
    }
    else {
        if (strcmp(command, "")!=0){
            if (strcmp(command, "run")==0){
                ptr_input->input_type=RUN;
                char job_name[10];
                char time[10];
                char priority[10];
                get_input(job_name);
                get_input(time);
                get_input(priority);
                // get_input(rest);
                if(strcmp(job_name, "")==0 || strcmp(time, "")==0 || strcmp(priority, "")==0){
                    ptr_input->input_type=ERROR;
                    printf("Invalid input: too few arguments.\nType 'help' to find more about AUbatch commands.\n");
                    return;
                }
                strcpy(ptr_input->job_name, job_name);
                ptr_input->time=atoi(time);
                ptr_input->priority=atoi(priority);
            }
            else if(strcmp(command, "fcfs")==0){ 
                ptr_input->input_type=CHANGE_POLICY;
                ptr_input->policy=FCFS;
            }
            else if(strcmp(command, "sjf")==0){
                ptr_input->input_type=CHANGE_POLICY;
                ptr_input->policy=SJF;
            }
            else if(strcmp(command, "priority")==0){
                ptr_input->input_type=CHANGE_POLICY;
                ptr_input->policy=PRIORITY;
            }
            else if (strcmp(command, "test")==0){
                ptr_input->input_type=TEST;
                char job_name[10];
                char policy[10];
                char num_of_jobs[10];
                char arrival_rate[10];
                char priority_levels[10];
                char min_CPU_time[10];
                char max_CPU_time[10];
                get_input(job_name);
                get_input(policy);
                get_input(num_of_jobs);
                get_input(arrival_rate);
                get_input(priority_levels);
                get_input(min_CPU_time);
                get_input(max_CPU_time);
                if(strcmp(job_name, "")==0 || strcmp(policy, "")==0
                   || strcmp(num_of_jobs, "")==0 || strcmp(arrival_rate, "")==0
                   || strcmp(priority_levels, "")==0 || strcmp(min_CPU_time, "")==0
                   || strcmp(max_CPU_time, "")==0){
                    ptr_input->input_type=ERROR;
                    printf("Invalid input: too few arguments.\nType 'help' to find more about AUbatch commands.\n");
                    return;
                }
                ptr_input->benchmark=malloc(sizeof(struct Benchmark));
                strcpy(ptr_input->benchmark->name, job_name);
                ptr_input->benchmark->policy=get_policy(policy);
                ptr_input->benchmark->num_of_jobs=atoi(num_of_jobs);
                ptr_input->benchmark->arrival_rate=atoi(arrival_rate);
                ptr_input->benchmark->priority_levels=atoi(priority_levels);
                ptr_input->benchmark->min_CPU_time=atoi(min_CPU_time);
                ptr_input->benchmark->max_CPU_time=atoi(max_CPU_time);
                if(ptr_input->benchmark->policy<0){
                    ptr_input->input_type=ERROR;
                    printf("Invalid input: command not supported.\nType 'help' to find more about AUbatch commands.\n");
                }
            }
            else{
                ptr_input->input_type=ERROR;
                printf("Invalid input: command not supported.\nType 'help' to find more about AUbatch commands.\n");
                return;
            }
        }
    }
    // printf("job name: %s\ninput type: %d\npolicy: %d\npriority: %d\ntime: %d\n", input_.job_name, input.input_type, input.policy, input.priority, input.time);
}
