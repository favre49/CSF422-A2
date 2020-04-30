#include "hashtable.h"

int board_equals(Board* b1, Board* b2)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(b1->board[i][j]!=b2->board[i][j])
                return 0;
        }
    }
    return 1;
}

hash_table* create_hash_table()
{
    hash_table* h = (hash_table*)malloc(sizeof(hash_table));
    h->buckets = (hash_table_bucket*)malloc(NUM_BUCKETS*sizeof(hash_table_bucket));

    // Initialize buckets of the hash table.
    for(int i = 0; i < NUM_BUCKETS; i++)
        h->buckets[i].head=NULL;

    // Initialize random bitstrings for Ziebrist hashing.
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            h->pos_keys[i][j] = rand();
        }
    }
    
    // Initialize count of values in the hash table.
    h->count=0;

    // Initialize lock.
    pthread_mutex_init(&(h->hash_table_lock),NULL);

    return h;
}

int hash(hash_table* h, Board* b)
{
    int val = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            val ^= h->pos_keys[4*i+j][b->board[i][j]-1];
        }
    }
    return val;
}

void insert(hash_table* h, Board* b)
{
    // Lock, to make this threadsafe.
    pthread_mutex_lock(&(h->hash_table_lock));
    int idx = hash(h,b)%NUM_BUCKETS;
    if (h->buckets[idx].head == NULL)
    {
        hash_table_item* temp = (hash_table_item*)malloc(sizeof(hash_table_item));
        temp->key = b;
        temp->next = NULL;
        h->buckets[idx].head = temp;
    }
    else
    {
        hash_table_item* curr = h->buckets[idx].head;
        while (curr->next != NULL)
            curr = curr->next;

        hash_table_item* temp = (hash_table_item*)malloc(sizeof(hash_table_item));
        temp->key = b;
        temp->next = NULL;

        curr->next = temp;
    }
    h->count++;
    pthread_mutex_unlock(&(h->hash_table_lock));
}

int exists(hash_table* h, Board* b)
{
    pthread_mutex_lock(&(h->hash_table_lock));
    // Checking if this exists.
    int idx = hash(h,b)%NUM_BUCKETS;
    if (h->buckets[idx].head != NULL)
    {
        hash_table_item* curr = h->buckets[idx].head;
        while (curr != NULL)
        {
            if (board_equals(curr->key,b))
            {
                pthread_mutex_unlock(&(h->hash_table_lock));
                return 1;
            }
            curr = curr->next;
        }
    }
    pthread_mutex_unlock(&(h->hash_table_lock));
    return 0;
}



