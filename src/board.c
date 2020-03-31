#include <windows.h>
#include "board.h"
#include "minesweeper_solver_utils.h"

const t_RGB_cell_averages cell_type_averages[] = {{{}, UNKNOWN_CELL},
                                                  {{}, 1},
                                                  {{}, 1},
                                                  {{}, 1},
                                                  {{}, 1},
                                                  {{}, 1},
                                                  {{}, 1},};

bool update_board(t_ptr_board board)
{
    bool is_game_over;
    t_screenshot_data screenshot_data = get_app_screenshot();
    set_board(board, screenshot_data);
    is_game_over = is_board_known(board) || is_game_failed(screenshot_data);
    free(screenshot_data.pixels);
    return is_game_over;
}

bool is_game_failed(t_screenshot_data screenshot_data)
{
    return false;
}

void set_board(t_ptr_board board, t_screenshot_data screenshot_data)
{
    for (unsigned short i = 0; i < board_size._x; i++)
        for (unsigned short j = 0; j < board_size._y; j++)
        {
            t_board_cell cell = {i, j};
            if (GET_CELL(board, cell) == UNKNOWN_CELL)
            {
                SET_CELL(board, cell, classify_cell(cell, screenshot_data));
            }
        }
}

unsigned short classify_cell(t_board_cell cell, t_screenshot_data screenshot_data)
{
    t_RGB_averages cell_RGB_averages = get_RGB_averages(cell, screenshot_data);
    return 0;
}

t_RGB_averages get_RGB_averages(t_board_cell cell, t_screenshot_data screenshot_data)
{
    t_RGB_averages RGB_averages = {0.0, 0.0, 0.0};
    for
    return RGB_averages;
}



