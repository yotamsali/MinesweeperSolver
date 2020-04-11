#ifndef MINESWEEPERSOLVER_BOARD_H
#define MINESWEEPERSOLVER_BOARD_H

#include <stdbool.h>
#include "error_codes.h"

typedef enum {
    EMPTY_CELL,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    UNKNOWN_CELL,
    MINE,
    NUMBER_OF_CELL_TYPES
} t_cell_type;

typedef enum {
    BLACK,
    DEFAULT_GREY,
    WHITE,
    DARK_GREY,
    RED,
    MAROON,
    GREEN,
    DARK_BLUE,
    BLUE,
    TURQUOISE,
    YELLOW,
    NUMBER_OF_COLORS
} t_color_name;

struct cell_rect {
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};
struct board_size {
    int _x;
    int _y;
};

typedef struct board_size t_board_size;
typedef struct board_size t_board_cell;
typedef t_cell_type *t_ptr_board;
typedef struct cell_rect t_cell_rect;
typedef float *t_color_histogram;


#define GET_CELL(board, cell) board[cell._x * board_size._x + cell._y]
#define SET_CELL(board, cell, value) board[cell._x * board_size._x + cell._y] = value

extern t_board_size board_size;

t_error_code update_board(t_ptr_board board, bool *is_game_over, t_cell_rect game_status_rect);

#endif //MINESWEEPERSOLVER_BOARD_H
