# Scheduler Upgrade: Multiple Data Structures

This document outlines the features required to upgrade the `aubatch` scheduler to a more efficient, OS-style architecture. The goal is to achieve instantaneous O(1) rescheduling by maintaining separate, optimized data structures for each scheduling policy. This project will use the TDD approach by writing a failing test and running it before implementing the function.

[x] **F1: Implement a Generic Heap Data Structure**
    [x] T1.1: Create new files `heap.c` and `heap.h`.
    [x] T1.2: The heap must be generic enough to function as both a Min-Heap (for SJF) and a Max-Heap (for Priority).
    [x] T1.3: Required functions: `heap_create()`, `heap_insert()`, `heap_extract_top()`, `heap_remove_at_index()`, and internal helpers like `heapify_up()` and `heapify_down()`.

[x] **F2: Update the `Job` Struct**
    [x] T2.1: Modify the `Job` struct in `job_storage.h`.
    [x] T2.2: Add members to store the job's current index within each heap (e.g., `int sjf_heap_index;`, `int priority_heap_index;`). This is crucial for enabling efficient O(log n) removal.
    [x] T2.3: Make Job a two way linked list, so that when poping with reference it is O(1)

[x] **F3: Integrate Data Structures into `JobStorage`**
    [x] T3.1: Modify the `JobStorage` struct in `job_storage.h`.
    [x] T3.2: Keep the `queue_head` for the FCFS linked list.
    [x] T3.3: Add pointers for the new data structures: `Heap* sjf_heap;` and `Heap* priority_heap;`.
    [x] T3.4: Update `setup_job_storage()` to initialize these new structures.

[x] **F4: Modify `add_to_queue()`**
    [x] T4.1: When a new job is submitted, it must be added to **all three** data structures:
        1. Appended to the FCFS linked list.
        2. Inserted into the SJF Min-Heap.
        3. Inserted into the Priority Max-Heap.
    [x] T4.2: The job's heap indices must be updated after each insertion.

[x] **F5: Overhaul `pop_queue()`**
    [x] T5.1: The function will check the current `job_storage->policy`.
    [x] T5.2: It will extract the next job from the **appropriate** data structure (head of the list for FCFS, top of the heap for SJF/Priority).
    [x] T5.3: **Crucially**, after a job is selected and removed from one structure, it must be efficiently removed from the other two.

[x] **F6: Simplify `reschedule()`**
    [x] T6.1: This function will be reduced to a single O(1) operation: changing the `policy` integer in the `JobStorage` struct.
    [x] T6.2: All the complex list-rebuilding logic will be removed.

[x] **F7: Update Build System**
    [x] T7.1: Modify the `makefile` to compile the new `heap.c` and link it into the final `aubatch` executable.

# Testing
make the test files with 
`make test`
run the test binary with
`./tests/test_heap`