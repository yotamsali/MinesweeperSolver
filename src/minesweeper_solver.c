#include <stdlib.h>
#include <stdio.h>
#include "minesweeper_solver_utils.h"
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

int start_game(t_level minesweeper_level) {
    t_ptr_board board = NULL;
    t_error_code error_code = RETURN_CODE_SUCCESS;
    board_size = minesweeper_level.board_size;
    error_code = raise_minesweeper();
    if (!error_code)
        return error_code;
    error_code = set_minesweeper_level(minesweeper_level);
    if (!error_code)
        return error_code;
    error_code = initialize_board_ptr(board);
    if (!error_code)
        return error_code;
    t_move move = get_first_move();
    while (true) {
        error_code = execute_move(move);
        if (!error_code)
            goto lblCleanup;
        bool is_game_over = false;
        error_code = update_board(board, &is_game_over);
        if (is_game_over || !error_code)
            goto lblCleanup;
        error_code = get_move(board, &move);
        if (!error_code){
            goto lblCleanup;
        }
    }
    lblCleanup:
    free(board);
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
    error_code = start_game(*minesweeper_level_ptr);
    return error_code;
    lblUsageError:
    printf(USAGE_MESSAGE);
    return error_code;
}
