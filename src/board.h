#ifndef MINESWEEPERSOLVER_BOARD_H
#define MINESWEEPERSOLVER_BOARD_H

#include "minesweeper_solver.h"

bool update_board(t_ptr_board board);
bool is_game_failed(BYTE * screenshot);
void set_board(t_ptr_board board, BYTE * screenshot);

#endif //MINESWEEPERSOLVER_BOARD_H
