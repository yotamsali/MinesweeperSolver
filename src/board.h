/**************************************************************************************************
 * @file board.h
 * @project MinesweeperSolver
 * @author Yotam Sali
 * @date 25.5.2020
 * @brief Header for board module, exports update_board and board data types.
*************************************************************************************************/
#ifndef MINESWEEPERSOLVER_BOARD_H
#define MINESWEEPERSOLVER_BOARD_H

#include <stdbool.h>
#include "error_codes.h"

/**
 * Enum for board cell types.
 */
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

/**
 * Enum for constant "magic" colors.
 * Those colors are used in cell detection.
 */
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

/**
 * Enum for game statuses (Smiley types).
 */
typedef enum {
    GAME_ON,
    WIN,
    LOST
} t_game_status;

/**
 * Struct for pixels indexes rectangle.
 */
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
typedef double *t_color_histogram;

/**
 * Macro for accessing board cell.
 */
#define BOARD_CELL(board, x_index, y_index) board[x_index * board_size.cols + y_index]

extern t_board_size board_size;

/**
 * @brief Update board state (cells) and status (smiley state).
 * The used technique is image processing over Minesweeper window screenshot.
 * For further details of implementation read board module.
 * @param board The board.
 * @param game_status Pointer for game status to update.
 * @param game_status_rect Pixels indexes rectangle for smiley (in window coordinates).
 * @return Error code.
 */
t_error_code update_board(t_board board, t_game_status *game_status, t_cell_rect game_status_rect);

#endif //MINESWEEPERSOLVER_BOARD_H
