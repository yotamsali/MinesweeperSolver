#ifndef MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H
#define MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H

#include "minesweeper_solver.h"

#define EXPERT_BOARD_LEVEL "expert"
#define INTERMEDIATE_BOARD_LEVEL "intermediate"
#define BEGINNER_BOARD_LEVEL "beginner"

#define INPUT_ERROR_MESSAGE "Input must be 'beginner', 'intermediate' or 'expert'."
#define NUMBER_ARGUMENT_MESSAGE "Number of arguments must be one."


void set_level_parameters(const char * level);
bool handle_input(int argc, char *argv[]);
void set_board_to_unknown(t_ptr_board board);
t_ptr_board initialize_board_ptr();
bool is_game_over(t_ptr_board board);
t_move get_first_move();

#endif //MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H
