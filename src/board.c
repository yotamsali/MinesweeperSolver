/**************************************************************************************************
 * @file board.c
 * @project MinesweeperSolver
 * @author Yotam Sali
 * @date 25.5.2020
 * @brief board module is responsible for Minesweeper board image detection.
 * Board image detection is being done in 3 major steps:
 * First, slice the board cells out of the Minesweeper window screenshot.
 * Second, create histogram of specific colors for every cell.
 * Third, use the cell to colors mapping to order to determine the kind of cell.
 * In addition, module allows to detects game status by detecting smiley type using color ratio.
*************************************************************************************************/
#include <windows.h>
#include <math.h>
#include <stdbool.h>
#include "error_codes.h"
#include "logger.h"
#include "common.h"
#include "board.h"
#include "commander.h"

/**
 * Macro to access a single pixel value by indexes and image size.
 */
#define GET_PIXEL_VALUE(pixels, x, y, width, height) *(pixels + ((height - y - 1) * width) + x)
/**
 * Macros for accessing R,G,B values from a single pixel pointer.
 */
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

/**
 * Storing R,G,B values of all constant "magic" colors.
 * Those colors are unique colors that help identifying cells.
 */
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

/**
 * Mapping between cell types with a pair of unique magic colors.
 * If cell pixels contains such a pair of colors, cell type is as declared.
 */
const t_unique_color_identifier unique_color_identifiers[NUMBER_OF_CELL_TYPES] =
        {{ONE,   {DEFAULT_GREY, BLUE}},
         {TWO,   {DEFAULT_GREY, GREEN}},
         {THREE, {DEFAULT_GREY, RED}},
         {FOUR,  {DEFAULT_GREY, DARK_BLUE}},
         {FIVE,  {DEFAULT_GREY, MAROON}},
         {SIX,   {DEFAULT_GREY, TURQUOISE}}};

/**
 * @brief Get cell pixels rectangle in window screenshot.
 * @param cell The cell.
 * @return Pixels rectangle that borders the cell in the screenshot.
 */
t_cell_rect get_cell_rect(t_board_cell cell) {
    t_cell_rect cell_rect = {(int) round(((double) (cell.row)) * BITMAP_CELL_SIZE) + X_BITMAP_MARGIN,
                             (int) round(((double) (cell.row + 1)) * BITMAP_CELL_SIZE) + X_BITMAP_MARGIN,
                             (int) round(((double) (cell.col)) * BITMAP_CELL_SIZE) + Y_BITMAP_MARGIN,
                             (int) round(((double) (cell.col + 1)) * BITMAP_CELL_SIZE) + Y_BITMAP_MARGIN};
    return cell_rect;
}

/**
 * @brief Extract R,G,B values of a pixel from image into struct.
 * @param screenshot_data Struct containing screenshot and size.
 * @param x X index of pixel.
 * @param y Y index of pixel.
 * @return Struct containing R,G,B values of pixel (x,y).
 */
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

/**
 * Equalizer between two colors structs.
 * @param color1 First color to equalize.
 * @param color2 Second color to equalize.
 * @return Boolean, true if color1 is color2, false otherwise.
 */
bool is_colors_equal(t_color color1, t_color color2) {
    return ((color1.R_value == color2.R_value) &&
            (color1.G_value == color2.G_value) &&
            (color1.B_value == color2.B_value));
}

/**
 * @brief Get color histogram of a cell.
 * The histogram contains the percentage of pixels from each "magic" unique color.
 * It turns that this color histogram of unique colors has a one-to-one mapping to cell type.
 * @param cell The cell.
 * @param screenshot_data Struct containing screenshot and size.
 * @return Cell's color histogram of unique constant colors.
 */
t_color_histogram get_cell_color_histogram(t_board_cell cell, t_screenshot_data *screenshot_data) {
    t_color_histogram histogram = (t_color_histogram) malloc(NUMBER_OF_COLORS * sizeof(double));
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
        histogram[j] /= (double) ((cell_rect.x_max - cell_rect.x_min) * (cell_rect.y_max - cell_rect.y_min));
    return histogram;
}

/**
 * @brief Predict cell type by a pair of unique colors.
 * Not all cell types contain a pair of unique colors that identifies them,
 * but many of them do. Those who do not continue to a more specific identification process.
 * @param histogram Color histogram of magic constant colors.
 * @param prediction Pointer to cell type prediction.
 * @return Boolean, true if prediction by pair of unique colors succeeded, false otherwise.
 */
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

/**
 * @brief Predict cell type by histogram.
 * This function is called for cells without a pair of unique color identifiers.
 * Such cell types are seven, eight, empty cell and unknown cell.
 * @param histogram Magic constant color histogram of cell.
 * @return Predicted cell type.
 */
t_cell_type predict_by_color_distribution(const t_color_histogram histogram) {
    if (histogram[BLACK] > SEVEN_BLACK_THRESHOLD)
        return SEVEN;
    else if (histogram[DARK_GREY] > EIGHT_DARK_GREY_THRESHOLD)
        return EIGHT;
    else if (histogram[WHITE] < WHITE_UNKNOWN_CELL_THRESHOLD)
        return EMPTY_CELL;
    return UNKNOWN_CELL;
}

/**
 * @brief Get cell type from screenshot.
 * @param prediction Pointer to predicted cell type.
 * @param cell Cell to predict.
 * @param screenshot_data_ptr Pointer to screenshot data (image and size).
 * @return Error code.
 */
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

/**
 * @brief Set board cells according to screenshot.
 * @param board Board to set.
 * @param screenshot_data_ptr Pointer to minesweeper window screenshot (image and size).
 * @return Error code.
 */
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

/**
 * @brief Update game status (smiley type), to determine whether we win, loose or continue playing.
 * Used technique is predicting based on yellow-black ratio of pixels in smiley pixels rectangle.
 * @param game_status Pointer to game status to update.
 * @param screenshot_data Pointer to screenshot (image and size).
 * @param game_status_rect Rectangle of smiley pixels indexes (changed between minesweeper levels).
 * @return Error code.
 */
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
    double black_yellow_ratio = (double) black_counter / (double) yellow_counter;
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