#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void run_for_time(time_t seconds) {
    time_t start_time = time(NULL);
    time_t current_time = start_time;

    while (current_time - start_time < seconds) {
        // Perform any necessary tasks here
        current_time = time(NULL);
    }
}

int main(int argc, char *argv[]) {
    time_t duration = atoi(argv[1]); // Run for 10 seconds
    run_for_time(duration);

    return 0;
}