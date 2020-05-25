/**
 * @file commander.h
 * @author Yotam Sali
 * @date 25.5.2020
 * @brief Header commander module. Exports Windows-based functions that are required to play Minesweeper.
 */
#ifndef MINESWEEPERSOLVER_COMMANDER_H
#define MINESWEEPERSOLVER_COMMANDER_H

#include "windows.h"
#include "board_analyzer.h"
#include "minesweeper_solver_utils.h"

struct screenshot_data {
    int width;
    int height;
    LPCOLORREF pixels;
};
typedef struct screenshot_data t_screenshot_data;

t_error_code raise_minesweeper();

t_error_code execute_moves(t_moves moves);

t_error_code set_minesweeper_level(t_level level);

t_error_code restart_game(t_level level);

t_error_code get_minesweeper_screenshot(t_screenshot_data *screenshot_data_ptr);

#endif //MINESWEEPERSOLVER_COMMANDER_H