#ifndef MINESWEEPERSOLVER_BOARD_H
#define MINESWEEPERSOLVER_BOARD_H

#include "commander.h"
#include "minesweeper_solver.h"

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
typedef struct RGB_averages t_RGB_averages;
typedef struct RGB_cell_averages t_RGB_cell_averages;

bool update_board(t_ptr_board board);
bool is_game_failed(t_screenshot_data screenshot_data);
void set_board(t_ptr_board board, t_screenshot_data screenshot);
unsigned short classify_cell(t_board_cell cell, t_screenshot_data screenshot_data);
t_RGB_averages get_RGB_averages(t_board_cell cell, t_screenshot_data screenshot_data);



#endif //MINESWEEPERSOLVER_BOARD_H
