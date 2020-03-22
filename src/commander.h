#ifndef MINESWEEPERSOLVER_COMMANDER_H
#define MINESWEEPERSOLVER_COMMANDER_H

#include "minesweeper_solver.h"

#define APP_NOT_RAISED_MESSAGE "Problem with raising the app. Verify MinesweeperX path is updated."

void raise_app();
COORD get_cursor_position(t_board_cell);
void click(COORD cursor_position, bool is_right_click);
void execute_move(t_move move);
void set_cursor_to_default();
HBITMAP get_app_screenshot();

#endif //MINESWEEPERSOLVER_COMMANDER_H