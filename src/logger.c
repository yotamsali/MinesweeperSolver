#include <stdio.h>
#include <time.h>
#include "logger.h"

#define DEBUG_TAG "Debug"
#define RUNTIME_TAG "Runtime"
#define BOARD_MAX_PRINTOUT_SIZE 2048
#define MAPS_MAX_PRINTOUT_SIZE 4096
#define MOVE_MAX_PRINTOUT_SIZE 100
#define HISTOGRAM_MAX_PRINTOUT_SIZE 512

int write_log(const char *tag, const char *message) {
    time_t now;
    time(&now);
    return printf("%s [%s]: %s\n", ctime(&now), tag, message);
}

//TODO: Implement Logging configuration in hard_coded_config (Debug & Runtime should be printed?)
t_error_code log_move(t_move move) {
    char move_buffer[MOVE_MAX_PRINTOUT_SIZE];
    snprintf(move_buffer, MOVE_MAX_PRINTOUT_SIZE, "Chosen move: (%d, %d), is mine = %d",
             move._cell._x, move._cell._y, move._is_mine);
    if (!write_log(RUNTIME_TAG, move_buffer))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

t_error_code log_game_status(float black_yellow_ratio, int yellow_counter, int black_counter, bool is_game_over) {
    char move_buffer[MOVE_MAX_PRINTOUT_SIZE];
    snprintf(move_buffer, MOVE_MAX_PRINTOUT_SIZE, "Is game over: %d, black yellow ratio %f, %d yellow, %d black",
             is_game_over, black_yellow_ratio, yellow_counter, black_counter);
    if (!write_log(DEBUG_TAG, move_buffer))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

t_error_code log_histogram(t_board_cell cell, t_color_histogram histogram) {
    char histogram_buffer[HISTOGRAM_MAX_PRINTOUT_SIZE];
    size_t current_buffer_length = 0;
    current_buffer_length += snprintf(histogram_buffer, HISTOGRAM_MAX_PRINTOUT_SIZE,
                                      "Histogram for cell (%d, %d):\n Histogram: ",
                                      cell._x, cell._y);
    for (int i = 0; i < NUMBER_OF_COLORS; i++) {
        current_buffer_length += snprintf(histogram_buffer + current_buffer_length,
                                          HISTOGRAM_MAX_PRINTOUT_SIZE - current_buffer_length, "%.3f ", histogram[i]);
    }
    if (!write_log(DEBUG_TAG, histogram_buffer))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

void write_board_matrix_to_buffer(char *buffer, void *matrix, size_t *writing_length,
                                  size_t buffer_size, bool is_float) {
    t_ptr_map float_map = NULL;
    t_ptr_board integer_board = NULL;
    if (is_float)
        float_map = (t_ptr_map) matrix;
    else
        integer_board = (t_ptr_board) matrix;
    for (int i = 0; i < board_size._x; i++) {
        for (int j = 0; j < board_size._y; j++) {
            t_board_cell cell = {j, i};
            if (is_float)
                *writing_length += snprintf(buffer + *writing_length, buffer_size - *writing_length,
                                            "%.3f ", GET_CELL(float_map, cell));
            else
                *writing_length += snprintf(buffer + *writing_length, buffer_size - *writing_length,
                                            "%d ", GET_CELL(integer_board, cell));
        }
        *writing_length += snprintf(buffer + *writing_length, buffer_size - *writing_length, "\n");
    }
}

t_error_code log_board(t_ptr_board board) {
    char board_buffer[BOARD_MAX_PRINTOUT_SIZE];
    size_t current_length = 0;
    current_length += snprintf(board_buffer, BOARD_MAX_PRINTOUT_SIZE, "Board detected:\n");
    write_board_matrix_to_buffer(board_buffer, board, &current_length, BOARD_MAX_PRINTOUT_SIZE, false);
    if (!write_log(RUNTIME_TAG, board_buffer))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

t_error_code log_probability_maps(t_ptr_map mine_map, t_ptr_map clear_map) {
    char maps_buffer[MAPS_MAX_PRINTOUT_SIZE];
    size_t current_length = 0;
    current_length += snprintf(maps_buffer, MAPS_MAX_PRINTOUT_SIZE, "Mine map:\n");
    write_board_matrix_to_buffer(maps_buffer, mine_map, &current_length, MAPS_MAX_PRINTOUT_SIZE, true);
    current_length += snprintf(maps_buffer + current_length, MAPS_MAX_PRINTOUT_SIZE, "Clear map:\n");
    write_board_matrix_to_buffer(maps_buffer, clear_map, &current_length, MAPS_MAX_PRINTOUT_SIZE, true);
    if (!write_log(RUNTIME_TAG, maps_buffer))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

// TODO: Implement file writing
t_error_code open_log() {
    return RETURN_CODE_SUCCESS;
}

t_error_code close_log() {
    return RETURN_CODE_SUCCESS;
}
