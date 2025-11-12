#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "evaluate.h"

#define ERROR -3
#define LIST -2
#define HELP -1
#define RUN 0
#define CHANGE_POLICY 1
#define TEST 2
#define QUIT 3

#define FCFS 0
#define SJF 1
#define PRIORITY 2

struct Input {
    int input_type;
    char job_name[50];
    char command[100];
    int policy;
    int priority;
    int time;
    struct Benchmark* benchmark;
}; 

void print_main_menu();

void get_input(char * command);

void print_help();

void setup_input(struct Input* ptr_input, char const* command);

void list_jobs(struct JobStorage* job_storage);

void quit_listing(struct JobStorage* job_storage);

void print_reschedule(struct JobStorage* job_storage);

void print_submit_job(struct JobStorage* job_storage, struct Job* job);

int get_policy(char const * policy);

void interface(struct Input* ptr_input);

#endif