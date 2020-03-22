#include "board.h"
#include "commander.h"

bool get_board(t_ptr_board board)
{
    HBITMAP screenshot = get_app_screenshot();
    free(screenshot);
    for (unsigned short i = 0; i < board_size._x; i++)
        for (unsigned short j = 0; j < board_size._y; j++)
        {
            t_board_cell cell = {i, j};
            SET_CELL(board, cell, UNKNOWN_CELL);
        }
}



