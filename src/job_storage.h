#ifndef JOB_STORAGE_H
#define JOB_STORAGE_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

struct Job {
   char name[50];
   struct Job* next_job;
   time_t raw_arrival_time;
   time_t raw_start_time;
   int time;
   int priority;
   bool is_benchmark;
}; 

struct JobStorage {
   struct Job* queue_head;
   struct Job* running_job;
   int policy;
   int count;
   int total_submitted;
   time_t raw_start_time;
   time_t raw_turnaround_time;
   time_t raw_waiting_time;
   time_t raw_cpu_time;
   int raw_expected_waiting_time;
}; 

void setup_job_storage(struct JobStorage* job_storage);

void setup_job(struct Job* job, char const* name, time_t arrival, int time, int priority, bool is_benchmark);

void use_policy(struct JobStorage* job_storage, struct Job* job);

bool add_to_queue(struct JobStorage* job_storage, struct Job* job);

void reschedule(struct JobStorage* job_storage, int policy);

struct Job* pop_queue(struct JobStorage* job_storage);

void job_done(struct JobStorage* job_storage, struct Job* job);

#endif
