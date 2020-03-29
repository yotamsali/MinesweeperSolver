#ifndef MINESWEEPERSOLVER_COMMANDER_H
#define MINESWEEPERSOLVER_COMMANDER_H

#include "minesweeper_solver.h"

#define APP_NOT_RAISED_MESSAGE "Problem with raising the app window. Verify MinesweeperX path and CPU load."
#define MOUSE_PROBLEM_MESSAGE "Problem with mouse control caused program termination."
#define SLEEP_RAISE_WINDOW_MILISECONDS 1500

#define BOARD_CELL_SIZE 16
#define BOARD_X_MARGIN 18
#define BOARD_Y_MARGIN 61

void raise_app();
POINT get_screen_cursor_position(t_board_cell move);
POINT get_app_cursor_position(t_board_cell move);
void click(POINT cursor_position, bool is_right_click);
void execute_move(t_move move);
void set_level_in_app();
HBITMAP get_app_screenshot_bitmap();
BYTE * get_app_screenshot();

#endif //MINESWEEPERSOLVER_COMMANDER_H