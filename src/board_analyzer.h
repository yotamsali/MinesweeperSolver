/**
 * @file board_analyzer.h
 * @author Yotam Sali
 * @date 25.5.2020
 * @brief Header for board_analyzer module. Exports get_moves.
 */
#ifndef MINESWEEPERSOLVER_BOARD_ANALYZER_H
#define MINESWEEPERSOLVER_BOARD_ANALYZER_H

#include <stdbool.h>
#include "board.h"

struct move {
    t_board_cell _cell;
    bool _is_mine;
};
typedef struct move t_move;
typedef float *t_ptr_map;

t_error_code get_move(t_ptr_board board, t_move *move, int total_number_of_mines);

#endif //MINESWEEPERSOLVER_BOARD_ANALYZER_H
