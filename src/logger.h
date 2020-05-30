/**************************************************************************************************
 * @file logger.h
 * @project MinesweeperSolver
 * @author Yotam Sali
 * @date 25.5.2020
 * @brief Header for logger module, exports log functions for specific purposes.
**************************************************************************************************/
#ifndef MINESWEEPERSOLVER_LOGGER_H
#define MINESWEEPERSOLVER_LOGGER_H

#include "error_codes.h"
#include "board_analyzer.h"
#include "board.h"
#include "matrix.h"

/**
 * @brief Log the game board state as detected.
 * @param board The board.
 * @return Error code of logging.
 */
t_error_code log_board(t_board board);

/**
 * @brief Log the matrix (of unknown cells linear equations).
 * @param matrix The matrix
 * @param message String of a logging message, representing matrix logging stage.
 * @return Error code of logging.
 */
t_error_code log_matrix(t_matrix matrix, const char *message);

/**
 * @brief Log variables map, mapping between board cell to variable index in the matrix.
 * @param variable_map Variables map pointer.
 * @return Error code of logging.
 */
t_error_code log_variables_map(t_matrix variables_map);

/**
 * @brief Log chosen game moves.
 * @param moves Pointer to chosen moves.
 * @return Error code of logging.
 */
t_error_code log_moves(t_moves moves);

/**
 * @brief Log game restarting (as a result of loosing the game).
 * @return Error code of logging.
 */
t_error_code log_game_restart();

/**
 * @brief Log game status detection process (smiley detection and result).
 * @param black_yellow_ratio Ratio between yellow and black pixels in smiley rect.
 * @param game_status Game status that was chosen.
 * @return Error code of logging.
 */
t_error_code log_game_status(double black_yellow_ratio, t_game_status game_status);

/**
 * @brief Log color histogram of a cell.
 * @param cell Board cell.
 * @param histogram Color histogram.
 * @return Error code of logging.
 */
t_error_code log_histogram(t_board_cell cell, t_color_histogram histogram);

/**
 * @brief Log illegal cell detection.
 * @param cell Illegal cell that was detected.
 * @return Error code of logging.
 */
t_error_code log_illegal_cell(t_board_cell cell);

/**
 * @brief Open log file (at the end of program runtime).
 * @return Error code of opening operation.
 */
t_error_code open_log();

/**
 * @brief Close log file (at the end of program runtime).
 * @return Error code of closure operation.
 */
t_error_code close_log();

#endif //MINESWEEPERSOLVER_LOGGER_H
