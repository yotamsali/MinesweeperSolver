#include <windows.h>
#include "board.h"
#include "commander.h"
#include "minesweeper_solver_utils.h"

bool update_board(t_ptr_board board)
{
    bool is_game_over;
    BYTE * app_screenshot = get_app_screenshot();
    set_board(board, app_screenshot);
    is_game_over = is_board_known(board) || is_game_failed(app_screenshot);
    return is_game_over;
}

bool is_game_failed(BYTE * app_screenshot)
{
    return false;
}

void set_board(t_ptr_board board, BYTE * screenshot)
{
    for (unsigned short i = 0; i < board_size._x; i++)
        for (unsigned short j = 0; j < board_size._y; j++)
        {
            t_board_cell cell = {i, j};
            SET_CELL(board, cell, UNKNOWN_CELL);
        }
}



