#ifndef MINESWEEPERSOLVER_BOARD_H
#define MINESWEEPERSOLVER_BOARD_H

#include <stdbool.h>
#include "error_codes.h"

#define UNKNOWN_CELL 9
#define EMPTY_CELL 0
#define MINE 10

struct board_size {
    unsigned short _x;
    unsigned short _y;
};

typedef struct board_size t_board_size;
typedef struct board_size t_board_cell;
typedef unsigned short *t_ptr_board;

#define GET_CELL(board, cell) board[cell._x * board_size._x + cell._y]
#define SET_CELL(board, cell, value) board[cell._x * board_size._x + cell._y] = value;
#define PIXEL_SIZE_IN_BYTES 4

extern t_board_size board_size;

t_error_code update_board(t_ptr_board board, bool *is_game_over);

#endif //MINESWEEPERSOLVER_BOARD_H
