#ifndef MINESWEEPERSOLVER_COMMANDER_H
#define MINESWEEPERSOLVER_COMMANDER_H

#include "minesweeper_solver.h"

#define APP_NOT_RAISED_MESSAGE "Problem with raising the app window. Verify MinesweeperX path and CPU load.\n"
#define SLEEP_RAISE_WINDOW_MILISECONDS 700
#define SLEEP_TO_SCREEN_UPDATE_MILISECONDS 200
#define BITMAP_INFORMATION_BIT_COUNT 32
#define BOARD_CELL_SIZE 16
#define BOARD_X_MARGIN 18
#define BOARD_Y_MARGIN 61
#define POINT_OUT_OF_BOARD_X -1
#define POINT_OUT_OF_BOARD_Y -1

struct screenshot_data
{
    int width;
    int height;
    BYTE * pixels;
};
typedef struct screenshot_data t_screenshot_data;

void raise_app();
POINT get_screen_cursor_position(t_board_cell move);
POINT get_app_cursor_position(t_board_cell move);
void click(POINT cursor_position, bool is_right_click);
void execute_move(t_move move);
void move_cursor_out_of_board();
void set_level_in_app();
t_screenshot_data get_app_screenshot();
void debug_save_bmp(BYTE * pixels, int width, int height, BITMAPINFOHEADER bitmap_information);


#endif //MINESWEEPERSOLVER_COMMANDER_H