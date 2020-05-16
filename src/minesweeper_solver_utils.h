#ifndef MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H
#define MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H

#include <windows.h>
#include "error_codes.h"
#include "board_analyzer.h"
#include "board.h"

struct t_level {
    char *level_name;
    t_board_size board_size;
    int x_button;
    int y_button;
    int number_of_mines;
    POINT point_game_restart;
    t_cell_rect game_status_rect;

};
typedef struct t_level t_level;

t_board initialize_board_ptr();

t_moves get_first_moves();

const t_level *get_level(char *level_arg);

#endif //MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H