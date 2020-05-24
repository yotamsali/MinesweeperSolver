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

typedef enum {
    GAME_ON,
    WIN,
    LOST
} t_game_status;

struct cell_rect {
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};
struct board_size {
    int rows;
    int cols;
};

struct board_cell {
    int row;
    int col;
};

typedef struct board_size t_board_size;
typedef struct board_cell t_board_cell;
typedef t_cell_type *t_board;
typedef struct cell_rect t_cell_rect;
typedef float *t_color_histogram;


#define BOARD_CELL(board, x_index, y_index) board[x_index * board_size.cols + y_index]

extern t_board_size board_size;

t_error_code update_board(t_board board, t_game_status *game_status, t_cell_rect game_status_rect);

#endif //MINESWEEPERSOLVER_BOARD_H
