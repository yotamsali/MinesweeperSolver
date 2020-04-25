#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "hard_coded_config.h"
#include "logger.h"

#define DEBUG_TAG "Debug"
#define RUNTIME_TAG "Runtime"
#define LOGGING_DIRECTORY_NAME "Logs"
#define MKDIR_MODE 0700
#define MAPS_MAX_PRINTOUT_SIZE 8192
#define BOARD_MAX_PRINTOUT_SIZE 2048
#define HISTOGRAM_MAX_PRINTOUT_SIZE 512
#define FILE_NAME_BUFFER_SIZE 128
#define GAME_STATUS_MAX_PRINTOUT_SIZE 128
#define MOVES_MAX_PRINTOUT_SIZE 2048

#define MOVE_TAG RUNTIME_TAG
#define GAME_STATUS_TAG DEBUG_TAG
#define GAME_RESTART_TAG RUNTIME_TAG
#define HISTOGRAM_TAG DEBUG_TAG
#define BOARD_TAG RUNTIME_TAG
#define MAPS_TAG DEBUG_TAG


FILE *log_file = NULL;

int write_log(const char *tag, const char *message) {
    time_t now;
    time(&now);
    return fprintf(log_file, "%s [%s]: %s\n", ctime(&now), tag, message);
}

bool is_logging_needed(const char *tag) {
    if (!strncmp(tag, DEBUG_TAG, sizeof(tag)) && !DEBUG_LOGGING)
        return false;
    if (!RUNTIME_LOGGING)
        return false;
    return true;
}

t_error_code log_moves(t_moves moves) {
    if (!is_logging_needed(MOVE_TAG))
        return RETURN_CODE_SUCCESS;
    char moves_buffer[MOVES_MAX_PRINTOUT_SIZE];
    size_t current_buffer_length = snprintf(moves_buffer, MOVES_MAX_PRINTOUT_SIZE, "Chosen moves: Number of moves - %d\n",
             moves.number_of_moves);
    for (int i = 0; i < moves.number_of_moves; i++) {
        current_buffer_length += snprintf(moves_buffer + current_buffer_length,
                MOVES_MAX_PRINTOUT_SIZE - current_buffer_length, "Move (%d, %d), is mine - %d\n",
                moves.moves[i].cell.x, moves.moves[i].cell.y, moves.moves[i].is_mine);
    }
    if (!write_log(MOVE_TAG, moves_buffer))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

const char *get_game_status_string(t_game_status status) {
    switch (status) {
        case WIN:
            return "Win";
        case LOST:
            return "Lost";
        default:
            return "Game on";
    }
}

t_error_code log_game_status(float black_yellow_ratio, t_game_status game_status) {
    if (!is_logging_needed(GAME_STATUS_TAG))
        return RETURN_CODE_SUCCESS;
    char move_buffer[GAME_STATUS_MAX_PRINTOUT_SIZE];
    snprintf(move_buffer, GAME_STATUS_MAX_PRINTOUT_SIZE, "Is game over: %s, black yellow ratio %f",
             get_game_status_string(game_status), black_yellow_ratio);
    if (!write_log(GAME_STATUS_TAG, move_buffer))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

t_error_code log_game_restart() {
    if (!is_logging_needed(GAME_RESTART_TAG))
        return RETURN_CODE_SUCCESS;
    if (!write_log(GAME_RESTART_TAG, "Restarting game due to lost.\n\n"))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

t_error_code log_histogram(t_board_cell cell, t_color_histogram histogram) {
    if (!is_logging_needed(HISTOGRAM_TAG))
        return RETURN_CODE_SUCCESS;
    char histogram_buffer[HISTOGRAM_MAX_PRINTOUT_SIZE];
    size_t current_buffer_length = 0;
    current_buffer_length += snprintf(histogram_buffer, HISTOGRAM_MAX_PRINTOUT_SIZE,
                                      "Histogram for cell (%d, %d):\n Histogram: ",
                                      cell.x, cell.y);
    for (int i = 0; i < NUMBER_OF_COLORS; i++) {
        current_buffer_length += snprintf(histogram_buffer + current_buffer_length,
                                          HISTOGRAM_MAX_PRINTOUT_SIZE - current_buffer_length, "%.3f ", histogram[i]);
    }
    if (!write_log(HISTOGRAM_TAG, histogram_buffer))
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
    for (int i = 0; i < board_size.y; i++) {
        for (int j = 0; j < board_size.x; j++) {
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
    if (!is_logging_needed(BOARD_TAG))
        return RETURN_CODE_SUCCESS;
    char board_buffer[BOARD_MAX_PRINTOUT_SIZE];
    size_t current_length = 0;
    current_length += snprintf(board_buffer, BOARD_MAX_PRINTOUT_SIZE, "Board detected:\n");
    write_board_matrix_to_buffer(board_buffer, board, &current_length, BOARD_MAX_PRINTOUT_SIZE, false);
    if (!write_log(BOARD_TAG, board_buffer))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

t_error_code log_probability_maps(t_ptr_map mine_map, t_ptr_map clear_map) {
    if (!is_logging_needed(MAPS_TAG))
        return RETURN_CODE_SUCCESS;
    char maps_buffer[MAPS_MAX_PRINTOUT_SIZE];
    size_t current_length = 0;
    current_length += snprintf(maps_buffer, MAPS_MAX_PRINTOUT_SIZE, "Mine map:\n");
    write_board_matrix_to_buffer(maps_buffer, mine_map, &current_length, MAPS_MAX_PRINTOUT_SIZE, true);
    current_length += snprintf(maps_buffer + current_length, MAPS_MAX_PRINTOUT_SIZE, "Clear map:\n");
    write_board_matrix_to_buffer(maps_buffer, clear_map, &current_length, MAPS_MAX_PRINTOUT_SIZE, true);
    if (!write_log(MAPS_TAG, maps_buffer))
        return ERROR_WRITE_LOG_FAILED;
    return RETURN_CODE_SUCCESS;
}

t_error_code open_log() {
    if (!RUNTIME_LOGGING && !DEBUG_LOGGING)
        return RETURN_CODE_SUCCESS;
    time_t now;
    struct tm *info;
    struct stat status = {0};
    time(&now);
    info = localtime(&now);
    char file_name_buffer[FILE_NAME_BUFFER_SIZE];
    size_t written_bytes = snprintf(file_name_buffer, FILE_NAME_BUFFER_SIZE, "%s\\", LOGGING_DIRECTORY_NAME);
    written_bytes += strftime(file_name_buffer + written_bytes, FILE_NAME_BUFFER_SIZE - written_bytes,
                              "%Y%m%d-%H%M%S.log", info);
    if (!written_bytes)
        return ERROR_CREATING_LOG_FILE_NAME;
    if (stat(LOGGING_DIRECTORY_NAME, &status) == -1) {
        if (mkdir(LOGGING_DIRECTORY_NAME, MKDIR_MODE))
            return ERROR_CREATING_LOGS_DIRECTORY;
    }
    log_file = fopen(file_name_buffer, "w");
    if (!log_file)
        return ERROR_OPENING_LOG_FILE;
    return RETURN_CODE_SUCCESS;
}

t_error_code close_log() {
    if (!RUNTIME_LOGGING && !DEBUG_LOGGING)
        return RETURN_CODE_SUCCESS;
    if (fclose(log_file))
        return ERROR_CLOSING_LOG_FILE;
    return RETURN_CODE_SUCCESS;
}
