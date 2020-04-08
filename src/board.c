#include <windows.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include "error_codes.h"
#include "common.h"
#include "board.h"
#include "commander.h"

struct RGB_averages {
    float R_average;
    float G_average;
    float B_average;
};
struct RGB_cell_averages {
    struct RGB_averages RGB_averages;
    unsigned short cell_type;
};
struct cell_rect {
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};
typedef struct RGB_averages t_RGB_averages;
typedef struct RGB_cell_averages t_RGB_cell_averages;
typedef struct cell_rect t_cell_rect;

#define GET_R_PIXEL_VALUE(screenshot_data, x, y) (BYTE)*(screenshot_data->pixels + (x * screenshot_data->width + y) * PIXEL_SIZE_IN_BYTES)
#define GET_G_PIXEL_VALUE(screenshot_data, x, y) (BYTE)*(screenshot_data->pixels + (x * screenshot_data->width + y) * PIXEL_SIZE_IN_BYTES + 1)
#define GET_B_PIXEL_VALUE(screenshot_data, x, y) (BYTE)*(screenshot_data->pixels + (x * screenshot_data->width + y) * PIXEL_SIZE_IN_BYTES + 2)
#define X_BITMAP_MARGIN 26
#define Y_BITMAP_MARGIN 99
#define BITMAP_CELL_SIZE 16.3

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


float cell_averages_distance(t_RGB_averages x, t_RGB_averages y) {
    return powf(x.R_average - y.R_average, 2)
           + powf(x.G_average - y.G_average, 2)
           + powf(x.B_average - y.B_average, 2);
}

t_cell_rect get_cell_rect(t_board_cell cell) {
    t_cell_rect cell_rect = {(int) round(((float) (cell._x)) * BITMAP_CELL_SIZE) + X_BITMAP_MARGIN,
                             (int) round(((float) (cell._x + 1)) * BITMAP_CELL_SIZE) + X_BITMAP_MARGIN,
                             (int) round(((float) (cell._y)) * BITMAP_CELL_SIZE) + Y_BITMAP_MARGIN,
                             (int) round(((float) (cell._y + 1)) * BITMAP_CELL_SIZE) + Y_BITMAP_MARGIN};
    return cell_rect;
}

t_RGB_averages get_RGB_averages(t_board_cell cell, t_screenshot_data *screenshot_data) {
    t_RGB_averages RGB_averages;
    t_cell_rect cell_rect = get_cell_rect(cell);
    ASSERT(cell_rect.x_min >= 0);
    ASSERT(cell_rect.y_min >= 0);
    ASSERT(cell_rect.x_max < screenshot_data -> width);
    ASSERT(cell_rect.y_max < screenshot_data -> height);
    float R_sum = 0, G_sum = 0, B_sum = 0;
    int number_of_cell_pixels = (cell_rect.x_max - cell_rect.x_min) * (cell_rect.y_max - cell_rect.x_min);
    for (int x = cell_rect.x_min; x < cell_rect.x_max; x++)
        for (int y = cell_rect.y_min; y < cell_rect.y_max; y++) {
            R_sum += GET_R_PIXEL_VALUE(screenshot_data, x, y);
            G_sum += GET_G_PIXEL_VALUE(screenshot_data, x, y);
            B_sum += GET_B_PIXEL_VALUE(screenshot_data, x, y);
        }
    RGB_averages.R_average = R_sum / (float) number_of_cell_pixels;
    RGB_averages.G_average = G_sum / (float) number_of_cell_pixels;
    RGB_averages.B_average = B_sum / (float) number_of_cell_pixels;
    return RGB_averages;
}

unsigned short classify_cell(t_board_cell cell, t_screenshot_data *screenshot_data_ptr) {
    float min_distance = FLT_MAX;
    int min_index = 0;
    t_RGB_averages current_RGB_cell_averages = get_RGB_averages(cell, screenshot_data_ptr);
    for (int i = 0; i < sizeof(cell_type_averages) / sizeof(t_RGB_cell_averages); i++) {
        float current_distance = cell_averages_distance(cell_type_averages[i].RGB_averages, current_RGB_cell_averages);
        if (current_distance < min_distance)
            min_index = i;
    }
    return cell_type_averages[min_index].cell_type;
}

void set_board(t_ptr_board board, t_screenshot_data *screenshot_data_ptr) {
    for (unsigned short i = 0; i < board_size._x; i++)
        for (unsigned short j = 0; j < board_size._y; j++) {
            t_board_cell cell = {i, j};
            if (GET_CELL(board, cell) == UNKNOWN_CELL)
                SET_CELL(board, cell, classify_cell(cell, screenshot_data_ptr));
        }
}

bool is_game_ended(t_screenshot_data *screenshot_data) {
    //TODO: Implement
    return false;
}

t_error_code update_board(t_ptr_board board, bool *is_game_over) {
    t_screenshot_data screenshot_data = {0, 0, NULL};
    t_error_code error_code = get_minesweeper_screenshot(&screenshot_data);
    if (error_code)
        return error_code;
    set_board(board, &screenshot_data);
    *is_game_over = is_game_ended(&screenshot_data);
    free(screenshot_data.pixels);
    return RETURN_CODE_SUCCESS;
}



