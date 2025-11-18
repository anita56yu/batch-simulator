#include "heap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // For debugging printf

// Helper functions for maintaining heap properties
static void heapify_up(Heap* heap, int index);
static void heapify_down(Heap* heap, int index);

static void heapify_up(Heap* heap, int index) {
    // printf("heapify_up: starting with index %d\n", index); fflush(stdout);
    int parent = (index - 1) / 2;
    while (index > 0 && heap->compare(heap->data[index], heap->data[parent]) < 0) {
        // printf("heapify_up: swapping index %d (value %d) with parent %d (value %d)\n", index, *(int*)heap->data[index], parent, *(int*)heap->data[parent]); fflush(stdout);
        void* temp = heap->data[index];
        heap->data[index] = heap->data[parent];
        heap->data[parent] = temp;

        if (heap->set_index) {
            heap->set_index(heap->data[index], index);
            heap->set_index(heap->data[parent], parent);
        }
        index = parent;
        parent = (index - 1) / 2;
    }
    // printf("heapify_up: finished\n"); fflush(stdout);
}

static void heapify_down(Heap* heap, int index) {
    int left_child = 2 * index + 1;
    int right_child = 2 * index + 2;
    int smallest_or_largest = index;

    if (left_child < heap->size && heap->compare(heap->data[left_child], heap->data[smallest_or_largest]) < 0) {
        smallest_or_largest = left_child;
    }

    if (right_child < heap->size && heap->compare(heap->data[right_child], heap->data[smallest_or_largest]) < 0) {
        smallest_or_largest = right_child;
    }

    if (smallest_or_largest != index) {
        void* temp = heap->data[index];
        heap->data[index] = heap->data[smallest_or_largest];
        heap->data[smallest_or_largest] = temp;

        if (heap->set_index) {
            heap->set_index(heap->data[index], index);
            heap->set_index(heap->data[smallest_or_largest], smallest_or_largest);
        }
        heapify_down(heap, smallest_or_largest);
    }
}

Heap* heap_create(int capacity, heap_compare_func compare, heap_set_index_func set_index) {
    Heap* heap = (Heap*)malloc(sizeof(Heap));
    if (!heap) {
        return NULL;
    }
    heap->data = (void**)malloc(sizeof(void*) * capacity);
    if (!heap->data) {
        free(heap);
        return NULL;
    }
    heap->capacity = capacity;
    heap->size = 0;
    heap->compare = compare;
    heap->set_index = set_index;
    return heap;
}

void heap_destroy(Heap* heap) {
    if (heap) {
        free(heap->data);
        free(heap);
    }
}

void heap_insert(Heap* heap, void* item) {
    // printf("heap_insert: inserting item %d, current size %d\n", *(int*)item, heap->size); fflush(stdout);
    if (heap->size == heap->capacity) {
        // printf("heap_insert: heap is full\n"); fflush(stdout);
        return;
    }
    heap->data[heap->size] = item;
    if (heap->set_index) {
        heap->set_index(item, heap->size);
    }
    heap->size++;
    heapify_up(heap, heap->size - 1);
    // printf("heap_insert: item %d inserted, new size %d\n", *(int*)item, heap->size); fflush(stdout);
}

void* heap_extract_top(Heap* heap) {
    if (heap->size == 0) {
        return NULL;
    }
    void* item = heap->data[0];
    if (heap->set_index) {
        heap->set_index(item, -1); // Indicate that the item is no longer in the heap
    }
    heap->size--;
    if (heap->size > 0) {
        heap->data[0] = heap->data[heap->size];
        if (heap->set_index) {
            heap->set_index(heap->data[0], 0);
        }
        heapify_down(heap, 0);
    }
    return item;
}

void heap_remove_at_index(Heap* heap, int index) {
}
