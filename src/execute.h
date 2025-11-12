#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "job_storage.h"

char* intToCharArray(int num);

void execute_job(struct Job* job);

void execute_benchmark(struct Job* job);

#endif
