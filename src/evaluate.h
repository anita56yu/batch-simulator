#ifndef EVALUATION_H
#define EVALUATION_H

#include <stdbool.h>
#include <time.h>
#include "job_storage.h"

struct Benchmark{
    char name[50];
    int policy;
    int new_policy;
    int num_of_jobs;
    int arrival_rate;
    int priority_levels;
    int min_CPU_time;
    int max_CPU_time;
};

void generate_job(struct Benchmark* benchmark, struct Job *job);

void run_for_time(time_t seconds);

#endif