#ifndef MINESWEEPERSOLVER_BOARD_H
#define MINESWEEPERSOLVER_BOARD_H

#include "minesweeper_solver.h"

#define cell(i, j, board_size) = (i * board_size->size_x + j) * sizeof(unsigned short)

bool get_board(t_ptr_board board);

#endif //MINESWEEPERSOLVER_BOARD_H
