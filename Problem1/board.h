#ifndef BOARD_H
#define BOARD_H

typedef struct Board Board;

/**
 * Struct defining a board configuration.
 *
 * board is a matrix defining the board configuration.
 * goodness is the "goodness" value of the matrix.
 * blank_x is the row where the blank exists.
 * blank_y is the column where the blank exists.
 * prev is a pointer to the previous board config so that we can backtrack and
 *      find the steps taken to solve it.
 */
struct Board{
    int board[4][4];
    int goodness;
    int blank_x;
    int blank_y;
    Board* prev;
};

#endif
