#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "board.h"

#define NUM_BUCKETS 1000

typedef struct hash_table_item hash_table_item;

struct hash_table_item{
    Board* key;
    hash_table_item* next;
};

struct hash_table_bucket {
    hash_table_item* head;
};

typedef struct hash_table_bucket hash_table_bucket;

typedef struct {
    int count;
    int pos_keys[16][16];
    hash_table_bucket* buckets;
    pthread_mutex_t hash_table_lock;
} hash_table;

// Create hash table.
hash_table* create_hash_table();

// Find the hash for a given board.
int hash(hash_table* h, Board* b);

// Insert into hash table.
void insert(hash_table* h, Board* b);

// Check if board exists in hash table.
int exists(hash_table* h, Board* b);


