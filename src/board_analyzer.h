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
    t_board_cell cell;
    bool is_mine;
};
typedef struct move t_move;
struct moves {
    t_move *moves;
    size_t number_of_moves;
};
typedef struct moves t_moves;

t_error_code get_moves(t_board board, t_moves *moves, int total_number_of_mines);

#endif //MINESWEEPERSOLVER_BOARD_ANALYZER_H
