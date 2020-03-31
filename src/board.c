#include <windows.h>
#include <math.h>
#include <float.h>
#include "board.h"
#include "minesweeper_solver_utils.h"

const t_RGB_cell_averages cell_type_averages[] = {{{0.0, 0.0, 0.0}, UNKNOWN_CELL},
                                                  {{0.0, 0.0, 0.0}, EMPTY_CELL},
                                                  {{0.0, 0.0, 0.0}, MINE},
                                                  {{0.0, 0.0, 0.0}, 1},
                                                  {{0.0, 0.0, 0.0}, 2},
                                                  {{0.0, 0.0, 0.0}, 3},
                                                  {{0.0, 0.0, 0.0}, 4},
                                                  {{0.0, 0.0, 0.0}, 5},
                                                  {{0.0, 0.0, 0.0}, 6},
                                                  {{0.0, 0.0, 0.0}, 7},
                                                  {{0.0, 0.0, 0.0}, 8}};

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
                SET_CELL(board, cell, classify_cell(cell, screenshot_data));
        }
}

unsigned short classify_cell(t_board_cell cell, t_screenshot_data screenshot_data)
{
    float min_distance = FLT_MAX;
    float current_distance;
    int min_index = 0;
    t_RGB_averages current_RGB_cell_averages = get_RGB_averages(cell, screenshot_data);
    for (int i = 0; i < sizeof(cell_type_averages) / sizeof(t_RGB_cell_averages); i++)
    {
        current_distance = cell_averages_distance(cell_type_averages[i].RGB_averages, current_RGB_cell_averages);
        if (current_distance < min_distance)
            min_index = i;
    }
    return cell_type_averages[min_index].cell_type;
}

float cell_averages_distance(t_RGB_averages x, t_RGB_averages y)
{
    return powf(x.R_average - y.R_average, 2)
    + powf(x.G_average - y.G_average, 2)
    + powf(x.B_average - y.B_average, 2);
}

t_RGB_averages get_RGB_averages(t_board_cell cell, t_screenshot_data screenshot_data)
{
    t_RGB_averages RGB_averages;
    t_cell_rect cell_rect = get_cell_rect(cell, screenshot_data.width, screenshot_data.height);
    int width = screenshot_data.width;
    int height = screenshot_data.height;
    float R_sum, G_sum, B_sum = 0;
    int number_of_cell_pixels = (cell_rect.x_max - cell_rect.x_min) * (cell_rect.y_max - cell_rect.x_min);
    for(int x = cell_rect.x_min; x < cell_rect.x_max; x++)
        for(int y = cell_rect.y_min; y < cell_rect.y_max; y++)
        {
            R_sum += GetRValue(GET_PIXEL(screenshot_data, x, y));
            G_sum += GetGValue(GET_PIXEL(screenshot_data, x, y));
            B_sum += GetBValue(GET_PIXEL(screenshot_data, x, y));
        }
    RGB_averages.R_average = R_sum / (float) number_of_cell_pixels;
    RGB_averages.G_average = G_sum / (float) number_of_cell_pixels;
    RGB_averages.B_average = B_sum / (float) number_of_cell_pixels;
    return RGB_averages;

}

t_cell_rect get_cell_rect(t_board_cell cell, int width, int height)
{
    t_cell_rect cell_rect;
    cell_rect.x_min = X_BITMAP_MARGIN;
    cell_rect.y_min = Y_BITMAP_MARGIN;
    cell_rect.x_max = (int)round((float)cell_rect.x_min + (float)cell._x * BITMAP_CELL_SIZE);
    cell_rect.y_max = (int)round((float)cell_rect.y_min + (float)cell._y * BITMAP_CELL_SIZE);
    return cell_rect;
}



