#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "job_storage.h"

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
}

//initializing a job
void setup_job(struct Job* job, char const* name, time_t arrival, int time, int priority, bool is_benchmark){
   strcpy(job->name, name);
   job->next_job=NULL;
   job->raw_arrival_time=arrival;
   job->time=time;
   job->priority=priority;
   job->is_benchmark=is_benchmark;
}

//append job to the queue in job_storage according to its policy
void use_policy(struct JobStorage* job_storage, struct Job* job){
   struct Job* privious = NULL;
   struct Job* current = job_storage -> queue_head;
   switch (job_storage->policy){
      case 0:
      // fcfs
         while(current!=NULL && difftime((current->raw_arrival_time), (job->raw_arrival_time))<0){
            privious = current;
            current = current -> next_job;
         }
         break;
      case 1:
      // sjf
         while(current!=NULL && (current->time)<(job->time)){
            privious = current;
            current = current -> next_job;
         }
         break;

      case 2:
      // priority
         while(current!=NULL && (current->priority)<(job->priority)){
            privious = current;
            current = current -> next_job;
         }
         break;

   }
   if (privious==NULL && current == NULL){
      job_storage -> queue_head=job;
   }
   else if(privious==NULL && current != NULL){
      job_storage -> queue_head=job;
      job->next_job=current;
   }
   else{
      privious -> next_job = job;
      job -> next_job = current;
   }
}

//add a job to the queue in job_stoage
//If the queue was empty return true, otherwise return false
bool add_to_queue(struct JobStorage* job_storage, struct Job* job){
   bool flag = false;
   struct Job* current = job_storage -> queue_head;
   if(current == NULL){
      job_storage -> queue_head = job;
   }
   else{
      use_policy(job_storage, job);
      flag=true;
   }
   job_storage->count++;
   job_storage->total_submitted++;
   job_storage->raw_expected_waiting_time+=job->time;
   return flag;
}

//reschedule the queue in job_storage according to new policy
void reschedule(struct JobStorage* job_storage, int policy){
   job_storage->policy = policy;
   struct Job* old_head = job_storage->queue_head;
   job_storage->queue_head=NULL;
   job_storage->total_submitted-=job_storage->count;
   job_storage->count=0;
   while(old_head!=NULL){
      struct Job* next = old_head->next_job;
      old_head->next_job=NULL;
      add_to_queue(job_storage, old_head);
      old_head = next;
   }
}

//pop the queue in job_storage and returns it
struct Job* pop_queue(struct JobStorage* job_storage){
   struct Job* head = job_storage -> queue_head;
   if (head!=NULL){
      job_storage -> queue_head = head -> next_job;
      job_storage->count--;
      head->raw_start_time=time(NULL);
      job_storage->raw_waiting_time+=head->raw_start_time-head->raw_arrival_time;
   }
   job_storage->running_job = head;
   return head;
}

//updates measurements of performance evaluation when a job is done
void job_done(struct JobStorage* job_storage, struct Job* job){
   job_storage->running_job=NULL;
   job_storage->raw_expected_waiting_time-=job->time;
   job_storage->raw_cpu_time+=time(NULL)-job->raw_start_time;
   job_storage->raw_turnaround_time+=time(NULL)-job->raw_arrival_time;
}
