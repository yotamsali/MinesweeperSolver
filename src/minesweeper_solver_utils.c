#include <stdio.h>
#include <stdlib.h>
#include "minesweeper_solver_utils.h"
#include "minesweeper_solver.h"

const t_board_size EXPERT_BOARD_SIZE = {16, 30};
const t_board_size INTERMEDIATE_BOARD_SIZE = {16, 16};
const t_board_size BEGINNER_BOARD_SIZE = {8, 8};

void set_level_parameters(const char * level)
{
    if (level == EXPERT_BOARD_LEVEL)
    {
        board_size = EXPERT_BOARD_SIZE;
        game_level = EXPERT_MARK;
    }
    else if (level == INTERMEDIATE_BOARD_LEVEL)
    {
        board_size = INTERMEDIATE_BOARD_SIZE;
        game_level = INTERMEDIATE_MARK;
    }
    else
    {
        board_size = BEGINNER_BOARD_SIZE;
        game_level = INTERMEDIATE_MARK;
    }
}

bool is_level_valid(const char * level)
{
    if (level != EXPERT_BOARD_LEVEL &&
    level != INTERMEDIATE_BOARD_LEVEL &&
    level != BEGINNER_BOARD_LEVEL)
        return false;
    return true;
}

bool handle_input(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf(NUMBER_ARGUMENT_MESSAGE);
        return false;
    }
    char * input_level = argv[0];
    if (!is_level_valid(input_level))
    {
        printf(INPUT_ERROR_MESSAGE);
        return false;
    }
    set_level_parameters(input_level);
    return true;
}

void set_board_to_unknown(t_ptr_board board)
{
    int board_cells_number = board_size._x * board_size._y;
    for (int i = 0; i < board_cells_number; i++)
        board[i] = UNKNOWN_CELL;
}

t_ptr_board initialize_board_ptr()
{
    unsigned short board_memory_size = board_size._x * board_size._y * sizeof(unsigned short);
    t_ptr_board ptr_board = (t_ptr_board*) malloc(board_memory_size);
    set_board_to_unknown(ptr_board);
    return ptr_board;
}

bool is_game_over(t_ptr_board board)
{
    return true;
}

t_move get_first_move()
{
    t_move first_move;
    t_board_cell first_move_cell = {board_size._x / 2, board_size._y / 2};
    first_move._is_mine = false;
    first_move._cell = first_move_cell;
    return first_move;
}



