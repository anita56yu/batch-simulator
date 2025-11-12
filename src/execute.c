#include "execute.h"

// Convert the integer to a string and store it in the char array
char* intToCharArray(int num) {
    char* result = (char*)malloc(20 * sizeof(char)); // Allocate memory for the char array
    sprintf(result, "%d", num); 
    return result;
}

// This function will execute file with a child process according to job 
void execute_job(struct Job* job){
    char *my_args[2];
    pid_t pid;

    my_args[0] = job->name;
    my_args[1] = NULL;

    switch ((pid = fork()))
    {
        case -1:
        /* Fork() has failed */
            perror("fork");
            break;
        case 0:
        /* This is processed by the child */
            execv(job->name, my_args);
            printf("[WARNING] Cannot execute file %s.\n", job->name);
            exit(EXIT_FAILURE);
            break;
        default:
        /* This is processed by the parent */
            wait(NULL);
            break;
    }
    free(my_args[1]);
}


// This function will execute file with a child process according to job 
void execute_benchmark(struct Job* job){
    char *my_args[3];
    pid_t pid;

    my_args[0] = job->name;
    my_args[1] = intToCharArray(job->time);
    my_args[2] = NULL;

    switch ((pid = fork()))
    {
        case -1:
        /* Fork() has failed */
            perror("fork");
            break;
        case 0:
        /* This is processed by the child */
            execv(job->name, my_args);
            printf("[WARNING] Cannot execute file %s.\n", job->name);
            exit(EXIT_FAILURE);
            break;
        default:
        /* This is processed by the parent */
            wait(NULL);
            break;
    }
    free(my_args[1]);
}