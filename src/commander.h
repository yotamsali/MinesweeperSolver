/**************************************************************************************************
 * @file commander.h
 * @project MinesweeperSolver
 * @author Yotam Sali
 * @date 25.5.2020
 * @brief Header commander module.
 * Exports functions that are required to play Minesweeper.
 * ALl functions require Windows OS (and uses windows.h and gdi32.dll).
 **************************************************************************************************/
#ifndef MINESWEEPERSOLVER_COMMANDER_H
#define MINESWEEPERSOLVER_COMMANDER_H

#include "windows.h"
#include "board_analyzer.h"
#include "minesweeper_solver_utils.h"

/**
 * Struct for containing data of screenshot (size and pointer to image data).
 */
struct screenshot_data {
    int width;
    int height;
    LPCOLORREF pixels;
};
typedef struct screenshot_data t_screenshot_data;

/**
 * @brief Raise Minesweeper game.
 * @return Error code.
 */
t_error_code raise_minesweeper();

/**
 * @brief Executes a series of moves using cursor.
 * @param moves Pointer to a series of moves.
 * @return Error code.
 */
t_error_code execute_moves(t_moves moves);

/**
 * @brief Set the minesweeper level by pressing correct the level button.
 * @param level Minesweeper level (can be beginner intermediate or expert).
 * @return Error code.
 */
t_error_code set_minesweeper_level(t_level level);

/**
 * @brief Restart game by pressing on the smiley button.
 * @param level Minesweeper level (can be beginner intermediate or expert).
 * @return Error code.
 */
t_error_code restart_game(t_level level);

/**
 * @brief Get screenshot of Minesweeper window (in Windows OS).
 * @param screenshot_data_ptr Pointer to screenshot data.
 * @return Error code.
 */
t_error_code get_minesweeper_screenshot(t_screenshot_data *screenshot_data_ptr);

#endif //MINESWEEPERSOLVER_COMMANDER_H