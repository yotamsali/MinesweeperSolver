#include <windows.h>
#include <math.h>
#include <stdbool.h>
#include "error_codes.h"
#include "logger.h"
#include "common.h"
#include "board.h"
#include "commander.h"

#define GET_PIXEL_VALUE(pixels, x, y, width, height) *(pixels + ((height - y - 1) * width) + x)
#define GET_R_VALUE(pixel) (LOBYTE((pixel)>>16))
#define GET_G_VALUE(pixel) (LOBYTE(((WORD)(pixel)) >> 8))
#define GET_B_VALUE(pixel) (LOBYTE(pixel))
#define X_BITMAP_MARGIN 14
#define Y_BITMAP_MARGIN 99
#define BITMAP_CELL_SIZE 16.0
#define UNIQUE_COLORS_NUMBER 2
#define EIGHT_DARK_GREY_THRESHOLD 0.3
#define WHITE_UNKNOWN_CELL_THRESHOLD 0.175
#define UNIQUE_COLOR_THRESHOLD 0.02
#define SEVEN_BLACK_THRESHOLD 0.01
#define MAX_BLACK_YELLOW_RATIO_GAME_ON 0.39
#define MAX_BLACK_YELLOW_RATIO_LOST 0.5

struct color {
    int R_value;
    int G_value;
    int B_value;
};

struct unique_color_identifier {
    t_cell_type cell_type;
    t_color_name unique_colors_sequence[UNIQUE_COLORS_NUMBER];
};

typedef struct cell_rect t_cell_rect;
typedef struct color t_color;
typedef struct unique_color_identifier t_unique_color_identifier;

const t_color colors_palette[NUMBER_OF_COLORS] =
        {{0,   0,   0},      // BLACK
         {192, 192, 192},    // DEFAULT_GREY
         {255, 255, 255},    // WHITE
         {128, 128, 128},    // DARK_GREY
         {255, 0,   0},      // RED
         {128, 0,   0},      // MAROON
         {0,   128, 0},      // GREEN
         {0,   0,   128},    // DARK_BLUE
         {0,   0,   255},    // BLUE
         {0,   128, 128},    // TURQUOISE
         {255, 255, 0}};     // YELLOW

const t_unique_color_identifier unique_color_identifiers[NUMBER_OF_CELL_TYPES] =
        {{ONE,   {DEFAULT_GREY, BLUE}},
         {TWO,   {DEFAULT_GREY, GREEN}},
         {THREE, {DEFAULT_GREY, RED}},
         {FOUR,  {DEFAULT_GREY, DARK_BLUE}},
         {FIVE,  {DEFAULT_GREY, MAROON}},
         {SIX,   {DEFAULT_GREY, TURQUOISE}}};

t_cell_rect get_cell_rect(t_board_cell cell) {
    t_cell_rect cell_rect = {(int) round(((float) (cell.row)) * BITMAP_CELL_SIZE) + X_BITMAP_MARGIN,
                             (int) round(((float) (cell.row + 1)) * BITMAP_CELL_SIZE) + X_BITMAP_MARGIN,
                             (int) round(((float) (cell.col)) * BITMAP_CELL_SIZE) + Y_BITMAP_MARGIN,
                             (int) round(((float) (cell.col + 1)) * BITMAP_CELL_SIZE) + Y_BITMAP_MARGIN};
    return cell_rect;
}

t_color get_pixel_color(t_screenshot_data *screenshot_data, int x, int y) {
    t_color pixel_color = {
            GET_R_VALUE(
                    GET_PIXEL_VALUE(screenshot_data->pixels, x, y, screenshot_data->width, screenshot_data->height)),
            GET_G_VALUE(
                    GET_PIXEL_VALUE(screenshot_data->pixels, x, y, screenshot_data->width, screenshot_data->height)),
            GET_B_VALUE(
                    GET_PIXEL_VALUE(screenshot_data->pixels, x, y, screenshot_data->width, screenshot_data->height))
    };
    return pixel_color;
}

bool is_colors_equal(t_color color1, t_color color2) {
    return ((color1.R_value == color2.R_value) &&
            (color1.G_value == color2.G_value) &&
            (color1.B_value == color2.B_value));
}

t_color_histogram get_cell_color_histogram(t_board_cell cell, t_screenshot_data *screenshot_data) {
    t_color_histogram histogram = (t_color_histogram) malloc(NUMBER_OF_COLORS * sizeof(float));
    for (int i = 0; i < NUMBER_OF_COLORS; i++)
        histogram[i] = 0;
    t_cell_rect cell_rect = get_cell_rect(cell);
    ASSERT(cell_rect.x_min >= 0);
    ASSERT(cell_rect.y_min >= 0);
    ASSERT(cell_rect.x_max < screenshot_data->width);
    ASSERT(cell_rect.y_max < screenshot_data->height);
    for (int x = cell_rect.x_min; x < cell_rect.x_max; x++)
        for (int y = cell_rect.y_min; y < cell_rect.y_max; y++) {
            t_color pixel_color = get_pixel_color(screenshot_data, x, y);
            for (int color_index = 0; color_index < NUMBER_OF_COLORS; color_index++) {
                if (is_colors_equal(pixel_color, colors_palette[color_index]))
                    histogram[color_index] += 1;
            }
        }
    for (int j = 0; j < NUMBER_OF_COLORS; j++)
        histogram[j] /= (float) ((cell_rect.x_max - cell_rect.x_min) * (cell_rect.y_max - cell_rect.y_min));
    return histogram;
}

bool predict_by_unique_color(const t_color_histogram histogram, t_cell_type *prediction) {
    for (int i = 0; i < sizeof(unique_color_identifiers) / sizeof(t_unique_color_identifier); i++) {
        t_unique_color_identifier color_identifier = unique_color_identifiers[i];
        bool is_color_matching = true;
        for (int j = 0; j < UNIQUE_COLORS_NUMBER; j++)
            if (histogram[color_identifier.unique_colors_sequence[j]] < UNIQUE_COLOR_THRESHOLD)
                is_color_matching = false;
        if (is_color_matching) {
            *prediction = color_identifier.cell_type;
            return true;
        }
    }
    return false;
}

t_cell_type predict_by_color_distribution(const t_color_histogram histogram) {
    if (histogram[BLACK] > SEVEN_BLACK_THRESHOLD)
        return SEVEN;
    else if (histogram[DARK_GREY] > EIGHT_DARK_GREY_THRESHOLD)
        return EIGHT;
    else if (histogram[WHITE] < WHITE_UNKNOWN_CELL_THRESHOLD)
        return EMPTY_CELL;
    return UNKNOWN_CELL;
}

t_error_code classify_cell(t_cell_type *prediction, t_board_cell cell, t_screenshot_data *screenshot_data_ptr) {
    t_color_histogram cell_histogram = get_cell_color_histogram(cell, screenshot_data_ptr);
    t_error_code error_code = log_histogram(cell, cell_histogram);
    if (error_code)
        return error_code;
    if (predict_by_unique_color(cell_histogram, prediction))
        goto lblCleanup;
    *prediction = predict_by_color_distribution(cell_histogram);
    lblCleanup:
    free(cell_histogram);
    return RETURN_CODE_SUCCESS;
}

t_error_code set_board(t_board board, t_screenshot_data *screenshot_data_ptr) {
    for (int row = 0; row < board_size.rows; row++)
        for (int col = 0; col < board_size.cols; col++) {
            t_board_cell cell = {row, col};
            if (BOARD_CELL(board, row, col) == UNKNOWN_CELL) {
                t_cell_type cell_prediction = UNKNOWN_CELL;
                t_error_code error_code = classify_cell(&cell_prediction, cell, screenshot_data_ptr);
                if (error_code)
                    return error_code;
                BOARD_CELL(board, row, col) = cell_prediction;
            }
        }
    return RETURN_CODE_SUCCESS;
}

t_error_code
update_game_status(t_game_status *game_status, t_screenshot_data *screenshot_data, t_cell_rect game_status_rect) {
    int black_counter = 0;
    int yellow_counter = 0;
    for (int x = game_status_rect.x_min; x < game_status_rect.x_max; x++)
        for (int y = game_status_rect.y_min; y < game_status_rect.y_max; y++) {
            t_color pixel_color = get_pixel_color(screenshot_data, x, y);
            if (is_colors_equal(pixel_color, colors_palette[YELLOW]))
                yellow_counter++;
            else if (is_colors_equal(pixel_color, colors_palette[BLACK]))
                black_counter++;
        }
    float black_yellow_ratio = (float) black_counter / (float) yellow_counter;
    if (black_yellow_ratio <= MAX_BLACK_YELLOW_RATIO_GAME_ON)
        *game_status = GAME_ON;
    else if (black_yellow_ratio <= MAX_BLACK_YELLOW_RATIO_LOST)
        *game_status = LOST;
    else
        *game_status = WIN;
    t_error_code error_code = log_game_status(black_yellow_ratio, *game_status);
    if (error_code)
        return error_code;
    return RETURN_CODE_SUCCESS;
}

t_error_code update_board(t_board board, t_game_status *game_status, t_cell_rect game_status_rect) {
    t_screenshot_data screenshot_data = {0, 0, NULL};
    t_error_code error_code = RETURN_CODE_SUCCESS;
    error_code = get_minesweeper_screenshot(&screenshot_data);
    if (error_code)
        return error_code;
    error_code = update_game_status(game_status, &screenshot_data, game_status_rect);
    if (*game_status != GAME_ON || error_code)
        goto lblCleanup;
    error_code = set_board(board, &screenshot_data);
    if (error_code)
        goto lblCleanup;
    error_code = log_board(board);
    if (error_code)
        goto lblCleanup;
    lblCleanup:
    free(screenshot_data.pixels);
    return error_code;
}