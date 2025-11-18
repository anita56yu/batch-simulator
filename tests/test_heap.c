#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/heap.h"

// Integer comparison function for the heap
int compare_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// A mock set_index function for testing purposes
void set_index_mock(void* data, int index) {
    // In a real scenario, this would update the object's index
}

void heap_print(Heap* heap) {
    printf("Heap (size %d, capacity %d): [", heap->size, heap->capacity); fflush(stdout);
    for (int i = 0; i < heap->size; i++) {
        printf("%d", *(int*)heap->data[i]); fflush(stdout);
        if (i < heap->size - 1) {
            printf(", "); fflush(stdout);
        }
    }
    printf("]\n"); fflush(stdout);
}

void test_heap_create() {
    printf("Running test: test_heap_create\n"); fflush(stdout);
    Heap* heap = heap_create(10, compare_ints, set_index_mock);
    assert(heap != NULL);
    assert(heap->capacity == 10);
    assert(heap->size == 0);
    assert(heap->compare == compare_ints);
    assert(heap->set_index == set_index_mock);
    heap_destroy(heap);
    printf("Passed test: test_heap_create\n"); fflush(stdout);
}

void test_heap_insert_and_extract() {
    printf("Running test: test_heap_insert_and_extract\n"); fflush(stdout);
    Heap* heap = heap_create(10, compare_ints, set_index_mock);
    int values[] = {10, 4, 15, 20, 0, 3};
    int sorted_values[] = {0, 3, 4, 10, 15, 20};
    int num_values = sizeof(values) / sizeof(int);

    for (int i = 0; i < num_values; i++) {
        heap_insert(heap, &values[i]);
        printf("After inserting %d: ", values[i]); fflush(stdout);
        heap_print(heap);
    }

    assert(heap->size == num_values);

    for (int i = 0; i < num_values; i++) {
        int* extracted = (int*)heap_extract_top(heap);
        printf("Extracted %d: ", *extracted); fflush(stdout);
        heap_print(heap);
        assert(*extracted == sorted_values[i]);
    }

    assert(heap->size == 0);
    heap_destroy(heap);
    printf("Passed test: test_heap_insert_and_extract\n"); fflush(stdout);
}


int main() {
    test_heap_create();
    test_heap_insert_and_extract();
    return 0;
}
