#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "board.h"
#include "board_analyzer.h"
#include "minesweeper_solver_utils.h"

const t_level levels[] = {{"expert",       {16, 30}, 5, 82,
                                  {242, 266, 63, 83}},
                          {"intermediate", {16, 16}, 5, 60,
                                  {132, 153, 63, 83}},
                          {"beginner",     {8,  8},  5, 38,
                                  {68,  90,  63, 83}}};

void set_board_to_unknown(t_ptr_board board) {
    int board_cells_number = board_size._x * board_size._y;
    for (int i = 0; i < board_cells_number; i++)
        board[i] = UNKNOWN_CELL;
}

t_ptr_board initialize_board_ptr() {
    int board_memory_size = board_size._x * board_size._y * sizeof(t_cell_type);
    t_ptr_board ptr_board = (t_ptr_board) malloc(board_memory_size);
    if (!ptr_board) {
        goto lblReturnPtr;
    }
    set_board_to_unknown(ptr_board);
    lblReturnPtr:
    return ptr_board;
}

t_move get_first_move() {
    t_move first_move;
    t_board_cell first_move_cell = {board_size._x / 2, board_size._y / 2};
    first_move._is_mine = false;
    first_move._cell = first_move_cell;
    return first_move;
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

