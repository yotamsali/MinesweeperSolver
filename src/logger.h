#ifndef MINESWEEPERSOLVER_LOGGER_H
#define MINESWEEPERSOLVER_LOGGER_H

#include "error_codes.h"
#include "board_analyzer.h"
#include "board.h"

t_error_code log_board(t_ptr_board board);

t_error_code log_move(t_move move);

t_error_code log_game_restart();

t_error_code log_game_status(float black_yellow_ratio, t_game_status game_status);

t_error_code log_histogram(t_board_cell cell, t_color_histogram);

t_error_code log_probability_maps(t_ptr_map mine_map, t_ptr_map clear_map);

t_error_code open_log();

t_error_code close_log();

#endif //MINESWEEPERSOLVER_LOGGER_H
