#ifndef MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H
#define MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H

#include "error_codes.h"
#include "board_analyzer.h"

struct t_level {
    char *level_name;
    t_board_size board_size;
    int x_button;
    int y_button;
};
typedef struct t_level t_level;

t_error_code initialize_board_ptr(t_ptr_board ptr_board);

t_move get_first_move();

const t_level *get_level(char *level_arg);

#endif //MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H