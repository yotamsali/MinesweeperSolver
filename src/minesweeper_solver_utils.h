#ifndef MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H
#define MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H

#include "minesweeper_solver.h"

#define EXPERT_BOARD_LEVEL "expert"
#define INTERMEDIATE_BOARD_LEVEL "intermediate"
#define BEGINNER_BOARD_LEVEL "beginner"

void set_level_global_parameters(const char * level);
void set_board_to_unknown(t_ptr_board board);
bool is_level_valid(const char * level);
t_ptr_board initialize_board_ptr();
bool is_board_known(t_ptr_board board);
t_move get_first_move();

#endif //MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H
