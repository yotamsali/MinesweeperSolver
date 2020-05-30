/**************************************************************************************
 * @file minesweeper_solver_utils.c
 * @project MinesweeperSolver
 * @author Yotam Sali
 * @date 25.5.2020
 * @brief Header for Minesweeper utilities. Exports level data and utility functions.
 *************************************************************************************/
#ifndef MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H
#define MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H

#include <windows.h>
#include "error_codes.h"
#include "board_analyzer.h"
#include "board.h"

/**
 * Minesweeper level parameters struct
 */
struct t_level {
    char *level_name;
    t_board_size board_size;
    int x_button; // X cursor coordinate of level button.
    int y_button; // Y cursor coordinate of level button.
    int number_of_mines;
    POINT point_game_restart; // Cursor point of game restart button.
    t_cell_rect game_status_rect; // Pixels indexes rectangle of smiley.

};
typedef struct t_level t_level;

/**
 * @brief Initialize board integer pointer in heap.
 * @return t_board (integer pointer), representing board with unknown cells.
 */
t_board initialize_board();

/**
 * @brief Get first move in game, which is pressing the middle cell.
 * @return t_moves struct. Contains the single move.
 */
t_moves get_first_moves();

/**
 * @brief Get level struct of requested level.
 * @param level_arg The program input string parameter of requested level.
 * @return Requested t_level struct of relevant level.
 */
const t_level *get_level(char *level_arg);

#endif //MINESWEEPERSOLVER_MINESWEEPER_SOLVER_UTILS_H