#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>

// A function pointer for comparing two elements in the heap.
// Should return a negative value if a < b, 0 if a == b, and a positive value if a > b.
typedef int (*heap_compare_func)(const void* a, const void* b);

// A function pointer to set the index of an element.
// This is useful for updating the position of an element in an auxiliary data structure.
typedef void (*heap_set_index_func)(void* data, int index);

// A generic heap data structure.
typedef struct Heap {
    void** data;
    int capacity;
    int size;
    heap_compare_func compare;
    heap_set_index_func set_index;
} Heap;

// Creates a new heap.
Heap* heap_create(int capacity, heap_compare_func compare, heap_set_index_func set_index);

// Destroys a heap.
void heap_destroy(Heap* heap);

// Inserts an element into the heap.
void heap_insert(Heap* heap, void* item);

// Extracts the top element from the heap.
void* heap_extract_top(Heap* heap);

// Removes an element at a specific index from the heap.
void heap_remove_at_index(Heap* heap, int index);

#endif // HEAP_H
