#include <stdbool.h>
#include <time.h>
#include "evaluate.h"
#include "job_storage.h"
#include <stdio.h>

//generate a job base on the given benchmark information including name, arrival_time, expected_execution_time, priority, is_benchmark
void generate_job(struct Benchmark* benchmark, struct Job *job){
    int diff = benchmark->max_CPU_time-benchmark->min_CPU_time;
    setup_job(job, benchmark->name, time(NULL), benchmark->min_CPU_time+(rand()%diff), rand()%benchmark->priority_levels, true);
}

//this function will run for seconds seconds
void run_for_time(time_t seconds) {
    time_t start_time = time(NULL);
    time_t current_time = start_time;

    while (current_time - start_time < seconds) {
        // Perform any necessary tasks here
        current_time = time(NULL);
    }
    printf("Ran for %ld seconds.\n", seconds); fflush(stdout);
}
