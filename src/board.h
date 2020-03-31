#ifndef MINESWEEPERSOLVER_BOARD_H
#define MINESWEEPERSOLVER_BOARD_H

#include "commander.h"
#include "minesweeper_solver.h"

#define GET_PIXEL(screenshot_data, x, y) screenshot_data.pixels[(x * screenshot_data.width + y)]
#define X_BITMAP_MARGIN 26
#define Y_BITMAP_MARGIN 99
#define BITMAP_CELL_SIZE 16.3

struct RGB_averages
{
    float R_average;
    float G_average;
    float B_average;
};
struct RGB_cell_averages
{
    struct RGB_averages RGB_averages;
    unsigned short cell_type;
};
struct cell_rect
{
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};
typedef struct RGB_averages t_RGB_averages;
typedef struct RGB_cell_averages t_RGB_cell_averages;
typedef struct cell_rect t_cell_rect;

bool update_board(t_ptr_board board);
bool is_game_failed(t_screenshot_data screenshot_data);
void set_board(t_ptr_board board, t_screenshot_data screenshot);
unsigned short classify_cell(t_board_cell cell, t_screenshot_data screenshot_data);
t_RGB_averages get_RGB_averages(t_board_cell cell, t_screenshot_data screenshot_data);
float cell_averages_distance(t_RGB_averages x, t_RGB_averages y);
t_cell_rect get_cell_rect(t_board_cell cell, int width, int height);


#endif //MINESWEEPERSOLVER_BOARD_H
