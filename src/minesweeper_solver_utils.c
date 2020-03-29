#include "minesweeper_solver_utils.h"
#include "minesweeper_solver.h"

const t_board_size EXPERT_BOARD_SIZE = {16, 30};
const t_board_size INTERMEDIATE_BOARD_SIZE = {16, 16};
const t_board_size BEGINNER_BOARD_SIZE = {8, 8};

void set_level_global_parameters(const char * level)
{
    if (strcmp(level, EXPERT_BOARD_LEVEL) == 0)
    {
        board_size = EXPERT_BOARD_SIZE;
        game_level_mark = EXPERT_MARK;
    }
    else if (strcmp(level, INTERMEDIATE_BOARD_LEVEL) == 0)
    {
        board_size = INTERMEDIATE_BOARD_SIZE;
        game_level_mark = INTERMEDIATE_MARK;
    }
    else
    {
        board_size = BEGINNER_BOARD_SIZE;
        game_level_mark = INTERMEDIATE_MARK;
    }
}

bool is_level_valid(const char * level)
{
    return  ((strcmp(level, EXPERT_BOARD_LEVEL) == 0) ||
            (strcmp(level, INTERMEDIATE_BOARD_LEVEL) == 0) ||
            (strcmp(level,BEGINNER_BOARD_LEVEL) == 0));
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
    t_ptr_board ptr_board = (t_ptr_board) malloc(board_memory_size);
    set_board_to_unknown(ptr_board);
    return ptr_board;
}

bool is_board_known(t_ptr_board board)
{
    for (int i = 0; i < board_size._x; i++)
        for (int j = 0; j < board_size._y; j++)
        {
            t_board_cell cell = {i, j};
            if (GET_CELL(board, cell) == UNKNOWN_CELL)
            {
                return false;
            }
        }
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



