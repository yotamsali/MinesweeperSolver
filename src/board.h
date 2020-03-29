#ifndef MINESWEEPERSOLVER_BOARD_H
#define MINESWEEPERSOLVER_BOARD_H

#include "minesweeper_solver.h"

bool update_board(t_ptr_board board);
bool is_game_failed(HBITMAP screenshot);
void set_board(t_ptr_board board, HBITMAP screenshot);

#endif //MINESWEEPERSOLVER_BOARD_H
