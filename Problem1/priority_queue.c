#include "priority_queue.h"

priority_queue* create_priority_queue()
{
    priority_queue* p = (priority_queue*)malloc(sizeof(priority_queue));
    p->heap_size = 0;
    pthread_mutex_init(&(p->priority_queue_lock),NULL);
    memset(&(p->heap), 0,sizeof(p->heap));
    return p;
}

void push(priority_queue* p, Board* b)
{
    pthread_mutex_lock(&(p->priority_queue_lock));
    if (p->heap_size==MAX_SIZE)
    {
        fprintf(stderr,"Priority queue is full!\n");
        exit(0);
    }

    p->heap_size++;
    int i = p->heap_size-1;
    p->heap[i]=b;

    while (i != 0 && p->heap[parent(i)]->goodness > p->heap[i]->goodness)
    {
        Board* temp = p->heap[i];
        p->heap[i] = p->heap[parent(i)];
        p->heap[parent(i)] = temp;
        i = parent(i);
    }
    pthread_mutex_unlock(&(p->priority_queue_lock));
}

Board* top(priority_queue* p)
{
    pthread_mutex_lock(&(p->priority_queue_lock));
    if (p->heap_size <= 0)
    {
        fprintf(stderr, "The priority queue is empty!!");
        exit(0);
    }

    if (p->heap_size == 1)
    {
        p->heap_size--;
        pthread_mutex_unlock(&(p->priority_queue_lock));
        return p->heap[0];
    }

    Board* root = p->heap[0];
    p->heap[0] = p->heap[p->heap_size-1];
    p->heap_size--;
    heapify(p,0);

    pthread_mutex_unlock(&(p->priority_queue_lock));
    return root;
}

void heapify(priority_queue* p, unsigned int i)
{
    unsigned int l = left(i);
    unsigned int r = right(i);
    int smallest = i;
    if (l < p->heap_size && p->heap[l]->goodness < p->heap[i]->goodness)
        smallest = l;
    if (r < p->heap_size && p->heap[r]->goodness < p->heap[smallest]->goodness)
        smallest = r;
    if (smallest != i)
    {
        Board* temp = p->heap[i];
        p->heap[i] = p->heap[smallest];
        p->heap[smallest] = temp;
        heapify(p, smallest);
    }
}

int is_empty(priority_queue* p)
{
    int val = 0;
    pthread_mutex_lock(&(p->priority_queue_lock));
    val = p->heap_size == 0;
    pthread_mutex_unlock(&(p->priority_queue_lock));
    return val;
}

unsigned int parent(unsigned int i)
{
    return (i-1)/2;
}

unsigned int left(unsigned int i)
{
    return 2*i+1;
}

unsigned int right(unsigned int i)
{
    return 2*i+2;
}

