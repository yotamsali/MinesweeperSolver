#include <stdlib.h>
#include <stdio.h>
#include "minesweeper_solver_utils.h"
#include "logger.h"
#include "commander.h"
#include "board.h"
#include "board_analyzer.h"
#include "error_codes.h"
#include "common.h"

// Input arguments, be careful when changing.
typedef enum {
    ARG_EXE_NAME = 0,
    ARG_GAME_LEVEL = 1,
    ARG_NUMBER // Number of argument (not arg index).
} t_arg;

t_board_size board_size = {0, 0};

#define USAGE_MESSAGE "Usage: MinesweeperSolver.exe level \n level - member of {beginner, intermediate, expert}\n"

t_error_code play_game(t_game_status *game_status, t_level minesweeper_level) {
    t_error_code error_code = RETURN_CODE_SUCCESS;
    t_board board = initialize_board(board);
    if (!board)
        return ERROR_INITIALIZE_BOARD_MEMORY;
    t_moves moves = get_first_moves();
    while (true) {
        error_code = execute_moves(moves);
        if (error_code)
            goto lblCleanup;
        error_code = update_board(board, game_status, minesweeper_level.game_status_rect);
        if (*game_status != GAME_ON || error_code)
            goto lblCleanup;
        error_code = get_moves(board, &moves, minesweeper_level.number_of_mines);
        if (error_code) {
            goto lblCleanup;
        }
    }
    lblCleanup:
    free(board);
    return error_code;
}

t_error_code start_game_trials(t_level minesweeper_level) {
    t_game_status game_status = GAME_ON;
    board_size = minesweeper_level.board_size;
    t_error_code error_code = raise_minesweeper();
    if (error_code)
        return error_code;
    error_code = set_minesweeper_level(minesweeper_level);
    if (error_code)
        return error_code;
    lblStartPlay:
    error_code = play_game(&game_status, minesweeper_level);
    if (game_status == LOST && !error_code) {
        error_code = restart_game(minesweeper_level);
        if (error_code)
            return error_code;
        error_code = log_game_restart();
        if (error_code)
            return error_code;
        goto lblStartPlay;
    }
    return error_code;
}

int main(int argc, char *argv[]) {
    t_error_code error_code = RETURN_CODE_SUCCESS;
    ASSERT(argv != NULL);
    if (argc != ARG_NUMBER) {
        error_code = ERROR_INCORRECT_USAGE_ARG_NUMBER;
        goto lblUsageError;
    }
    const t_level *minesweeper_level_ptr = get_level(argv[ARG_GAME_LEVEL]);
    if (minesweeper_level_ptr == NULL) {
        error_code = ERROR_INCORRECT_USAGE_ILLEGAL_LEVEL;
        goto lblUsageError;
    }
    error_code = open_log();
    if (error_code)
        goto lblReturn;
    error_code = start_game_trials(*minesweeper_level_ptr);
    if (error_code)
        goto lblReturn;
    error_code = close_log();
    if (error_code)
        goto lblReturn;
    lblReturn:
    return error_code;
    lblUsageError:
    printf(USAGE_MESSAGE);
    return error_code;

}
