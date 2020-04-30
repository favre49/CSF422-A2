#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "board.h"
#include "priority_queue.h"
#include "hashtable.h"

/*
 * Global constants.
 *
 * SOLVED is an array containing the correct indices for the tiles, to help in
 * calculation of the "goodness" values.
 *
 * BLANK is the representation of the blank tile. It is 16.
 */
const int SOLVED[16][2] = {{0,0},{0,1},{0,2},{0,3},
                           {1,0},{1,1},{1,2},{1,3},
                           {2,0},{2,1},{2,2},{2,3},
                           {3,0},{3,1},{3,2},{3,3}};

const int BLANK = 16;

/*
 * Global Data Structures.
 *
 * h is a pointer to the "closed list", implemented as a hash table.
 *
 * p is a pointer to the priority queue, which is the "open list".
 */
hash_table* h = NULL;
priority_queue* p = NULL;

/**
 * Global integers for timing purposes.
 *
 * start indicates the start time of the process.
 *
 * end indicates the end time of the process.
 */
clock_t start;
clock_t end;

/**
 * Global locks.
 *
 * solved_lock ensures that only one thread provides a solution.
 */
pthread_mutex_t solved_lock;

/*
 * Get the "goodness" of a board, defined as the sum of Manhattan distances of 
 * every tile from its correct position.
 */
int get_goodness(int arr[][4])
{
    int goodness = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int tile = arr[i][j];
            goodness += abs(i-SOLVED[tile-1][0]) + abs(j-SOLVED[tile-1][1]) ;
        }
    }
    return goodness;
}

/*
 * Check if a board is solved.
 */
int check_solved(Board* b)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int tile = b->board[i][j];
            if (i != SOLVED[tile-1][0] || j != SOLVED[tile-1][1])
                return 0;
        }
    }
    return 1;
}

// Prints the board.
void print_board(Board* b)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%d\t",b->board[i][j]);
        }
        printf("\n");
    }
    printf("***\n");
}

// Prints backtrace of a board.
void print_backtrace(Board* b)
{
    if (b->prev == NULL)
    {
        print_board(b);
        return;
    }
    else
    {
        print_backtrace(b->prev);
        print_board(b);
    }
}

/*
 * The function executed by the parallel threads.
 */
void* parallelThread(void* arg)
{
    int thread_id = (int)(long long)(arg);
    while(1)
    {
        Board* current_board;

        if (is_empty(p))
        {
            continue;
        }
        current_board = top(p);

        // Move blank up.
        if (current_board->blank_x > 0)
        {
            int newpos = current_board->blank_x - 1;
            Board* new_board = malloc(sizeof(Board));
            memcpy(new_board, current_board, sizeof(*current_board));
            new_board->board[current_board->blank_x][current_board->blank_y] = new_board->board[newpos][current_board->blank_y];
            new_board->board[newpos][current_board->blank_y] = BLANK;
            new_board->blank_x = newpos;

            new_board->goodness = get_goodness(new_board->board);
            new_board->prev = current_board;

            if (check_solved(new_board))
            {
                pthread_mutex_lock(&solved_lock);
                end = clock();
                print_backtrace(new_board);
                printf("Solved in %f seconds",(float)(end - start)/CLOCKS_PER_SEC);
                exit(0);
            }

            if (!exists(h,new_board))
            {
                insert(h,new_board);
                push(p,new_board);
            }
            else
                free(new_board);
        }

        // Move blank down.
        if (current_board->blank_x < 3)
        {
            int newpos = current_board->blank_x + 1;
            Board* new_board = malloc(sizeof(Board));
            memcpy(new_board, current_board, sizeof(*current_board));
            new_board->board[current_board->blank_x][current_board->blank_y] = new_board->board[newpos][current_board->blank_y];
            new_board->board[newpos][current_board->blank_y] = BLANK;
            new_board->blank_x = newpos;

            new_board->goodness = get_goodness(new_board->board);
            new_board->prev = current_board;

            if (check_solved(new_board))
            {
                pthread_mutex_lock(&solved_lock);
                end = clock();
                print_backtrace(new_board);
                printf("Solved in %f seconds",(float)(end - start)/CLOCKS_PER_SEC);
                exit(0);
            }

            if (!exists(h,new_board))
            {
                insert(h,new_board);
                push(p,new_board);
            }
            else
                free(new_board);
        }

        // Move blank right.
        if (current_board->blank_y < 3)
        {
            int newpos = current_board->blank_y + 1;
            Board* new_board = malloc(sizeof(Board));
            memcpy(new_board, current_board, sizeof(*current_board));
            new_board->board[current_board->blank_x][current_board->blank_y] = new_board->board[current_board->blank_x][newpos];
            new_board->board[current_board->blank_x][newpos] = BLANK;
            new_board->blank_y = newpos;

            new_board->goodness = get_goodness(new_board->board);
            new_board->prev = current_board;

            if (check_solved(new_board))
            {
                pthread_mutex_lock(&solved_lock);
                end = clock();
                print_backtrace(new_board);
                printf("Solved in %f seconds",(float)(end - start)/CLOCKS_PER_SEC);
                exit(0);
            }

            if (!exists(h,new_board))
            {
                insert(h,new_board);
                push(p,new_board);
            }
            else
                free(new_board);
        }

        // Move blank left.
        if (current_board->blank_y > 0)
        {
            int newpos = current_board->blank_y - 1;
            Board* new_board = malloc(sizeof(Board));
            memcpy(new_board, current_board, sizeof(*current_board));
            new_board->board[current_board->blank_x][current_board->blank_y] = new_board->board[current_board->blank_x][newpos];
            new_board->board[current_board->blank_x][newpos] = BLANK;
            new_board->blank_y = newpos;

            new_board->goodness = get_goodness(new_board->board);
            new_board->prev = current_board;

            if (check_solved(new_board))
            {
                pthread_mutex_lock(&solved_lock);
                end = clock();
                print_backtrace(new_board);
                printf("Solved in %f seconds",(float)(end - start)/CLOCKS_PER_SEC);
                exit(0);
            }

            if (!exists(h,new_board))
            {
                insert(h,new_board);
                push(p,new_board);
            }
            else
                free(new_board);
        }
    }

    return NULL;
}

int main(int argc, char** argv)
{
    int numThreads = 1;
    if (argc > 1)
        numThreads = atoi(argv[1]);

    // Read input.
    FILE* fptr;
    fptr = fopen("tests/input.txt","r");
    if (fptr == NULL)
    {
        fprintf(stderr,"input.txt does not exist!");
        exit(0);
    }

    int initial_pos[4][4];
    int initial_board_x;
    int initial_board_y;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            fscanf(fptr,"%d",&initial_pos[i][j]);
            if (initial_pos[i][j] == BLANK)
            {
                initial_board_x = i;
                initial_board_y = j;
            }
        }
    }

    // Get initial goodness.
    int initial_goodness = get_goodness(initial_pos);

    // Create initial board position.
    Board* initial_board = malloc(sizeof(Board));
    initial_board->goodness = initial_goodness;
    initial_board->blank_x = initial_board_x;
    initial_board->blank_y = initial_board_y;
    initial_board->prev = NULL;
    memcpy(initial_board->board,initial_pos,sizeof(initial_pos));

    if (check_solved(initial_board))
    {
        printf("The initial board is solved!\n");
        return 0;
    }

    // Start timing.
    start = clock();

    // Create hash table and priority queue.
    h = create_hash_table();
    p = create_priority_queue();

    // Insert first board in.
    insert(h,initial_board);
    push(p,initial_board);

    // Initialize locks.
    pthread_mutex_init(&solved_lock,NULL);

    // Create threads.
    pthread_t threads[numThreads];
    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, &parallelThread, (void*)(long long)i);
    }

    // Join!
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
}
