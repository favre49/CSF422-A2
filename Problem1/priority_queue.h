#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "board.h"

// Maximum size of the heap.
#define MAX_SIZE 50000

// Struct defining a priority queue (implemented as a min heap)
typedef struct {
    Board* heap[MAX_SIZE];
    int heap_size;
    pthread_mutex_t priority_queue_lock;
} priority_queue;

// Create a new priority queue.
priority_queue* create_priority_queue();

// Insert a board into the queue.
void push(priority_queue* p, Board* b);

// Get the top board of the queue.
Board* top(priority_queue* p);

// A recursive function to maintain the min heap property.
void heapify(priority_queue* p, unsigned int i);

// A function that checks if the heap is empty.
int is_empty(priority_queue* p);

// Utility function to find the parent of i.
unsigned int parent(unsigned int i);

// Utility function to find the left child of i.
unsigned int left(unsigned int i);

// Utility function to find the right child of i.
unsigned int right(unsigned int i);
