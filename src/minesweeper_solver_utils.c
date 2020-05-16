#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "board.h"
#include "board_analyzer.h"
#include "minesweeper_solver_utils.h"

const t_level levels[] = {{"expert",       {30, 16}, 5, 82,
                                  99, {245, 28},
                                  {242, 266, 63, 83}},
                          {"intermediate", {16, 16}, 5, 60,
                                  40, {132, 28},
                                  {132, 153, 63, 83}},
                          {"beginner",     {8,  8},  5, 38,
                                  10, {66,  28},
                                  {68,  90,  63, 83}}};

void set_board_to_unknown(t_board board) {
    int board_cells_number = board_size.x * board_size.y;
    for (int i = 0; i < board_cells_number; i++)
        board[i] = UNKNOWN_CELL;
}

t_board initialize_board_ptr() {
    int board_memory_size = board_size.x * board_size.y * sizeof(t_cell_type);
    t_board ptr_board = (t_board) malloc(board_memory_size);
    if (!ptr_board) {
        goto lblReturnPtr;
    }
    set_board_to_unknown(ptr_board);
    lblReturnPtr:
    return ptr_board;
}

t_moves get_first_moves() {
    t_move *first_move = (t_move *) malloc(sizeof(t_move));
    t_board_cell first_move_cell = {board_size.x / 2, board_size.y / 2};
    first_move->is_mine = false;
    first_move->cell = first_move_cell;
    t_moves moves = {first_move, 1};
    return moves;
}

const t_level *get_level(char *level_arg) {
    for (int i = 0; i < sizeof(levels) / sizeof(t_level); i++) {
        const t_level *level_ptr = &levels[i];
        if (!strncmp(level_ptr->level_name, level_arg, sizeof(level_ptr->level_name))) {
            return level_ptr;
        }
    }
    return NULL;
}

