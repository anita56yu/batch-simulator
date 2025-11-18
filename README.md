# aubatch Scheduler Upgrade

## Project Overview

This project upgrades `aubatch`, a simple batch job scheduling simulator. The primary goal was to refactor the core scheduling logic from an inefficient list-based system to a modern, OS-style architecture that utilizes multiple, policy-specific data structures. This change allows for instantaneous (`O(1)`) rescheduling between different scheduling policies.

The implemented scheduling policies are:
*   **FCFS (First-Come, First-Served):** Jobs are processed in the order they arrive.
*   **SJF (Shortest Job First):** The job with the shortest execution time is processed next.
*   **Priority:** The job with the highest priority is processed next.

## The Problem: Inefficient Rescheduling

The original implementation used a single linked list to store all waiting jobs. When the scheduling policy was changed (e.g., from FCFS to SJF), the entire list had to be traversed and rebuilt according to the new policy's rules. This resulted in a rescheduling time complexity of `O(n^2)`, which is highly inefficient for a large number of jobs.

## The Solution: Multiple Data Structures

To solve this, the scheduler was redesigned to maintain three separate data structures simultaneously:

1.  **Doubly Linked List:** For the FCFS policy.
2.  **Min-Heap:** For the SJF policy, ordered by job execution time.
3.  **Max-Heap:** For the Priority policy, ordered by job priority.

When a new job is submitted, it is added to all three data structures. When a job is selected for execution, it is removed from all three. This architecture reduces the `reschedule` operation to a single step: simply changing a pointer to the currently active policy. This makes rescheduling an instantaneous `O(1)`. operation.

## Performance Gains: Rescheduling Speedup

Micro-benchmarks were conducted to measure the time taken to reschedule a large number of waiting jobs from FCFS to another policy. The results demonstrate a dramatic performance improvement.

With a queue of 10,000 jobs, the following speedups were observed:

*   **FCFS -> SJF:** The rescheduling time was reduced from **0.154861 seconds** to **0.000006 seconds**, achieving a speedup of approximately **25,810x**.
*   **FCFS -> Priority:** The rescheduling time was reduced from **0.059649 seconds** to **0.000002 seconds**, achieving a speedup of approximately **29,824x**.

These results confirm the significant efficiency gains of the new architecture.

## How to Build and Run

### Build the Project

To compile the `aubatch` executable and all related components, run:
```bash
make all
```

### Run the `aubatch` Binary

To run the `aubatch` scheduler, use the following command format:

```bash
./aubatch <input_file>
```

Where `<input_file>` is a text file containing job definitions. Each line in the input file represents a job and should follow this format:

```
<job_name> <arrival_time> <burst_time> <priority>
```

*   `<job_name>`: A string identifier for the job (e.g., `jobA`, `task1`).
*   `<arrival_time>`: The time (an integer) at which the job arrives in the system.
*   `<burst_time>`: The CPU burst time (an integer) required for the job's execution.
*   `<priority>`: The priority level (an integer) of the job. Higher numbers typically indicate higher priority.

**Example `input.txt`:**
```
run job1 0 10 5
run job2 2 5 10
run job3 4 8 2
```

### Run Performance Benchmarks

To calculate the performance of a set of jobs and measure rescheduling times, use the `batch_job` executable with the following format:

```bash
test batch_job <initial_policy> <num_jobs> <min_time> <max_time> <min_priority> <max_priority> <reschedule_policy>
```

*   `<initial_policy>`: The initial scheduling policy (e.g., `fcfs`).
*   `<num_jobs>`: The number of benchmark jobs to generate and submit.
*   `<min_time>`: The minimum burst time for generated jobs.
*   `<max_time>`: The maximum burst time for generated jobs.
*   `<min_priority>`: The minimum priority for generated jobs.
*   `<max_priority>`: The maximum priority for generated jobs.
*   `<reschedule_policy>`: The policy to switch to for rescheduling (e.g., `sjf`, `priority`).

**Example:**
```bash
test batch_job fcfs 10000 3 3 2 10 priority
```
This command will run a benchmark starting with FCFS, generating 10,000 jobs with burst times between 3 and 3, priorities between 2 and 10, and then reschedule to the PRIORITY policy.

### Run the Test Suite

A test suite for the generic heap implementation is included. To build and run the tests, use:
```bash
make test
```
