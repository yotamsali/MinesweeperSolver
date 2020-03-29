#include "minesweeper_solver_utils.h"
#include "commander.h"
#include "board.h"
#include "board_analyzer.h"
#include "minesweeper_solver.h"

void start_game()
{
    bool is_game_over;
    raise_app();
    set_level_in_app();
    t_ptr_board board = initialize_board_ptr(board_size);
    t_move move = get_first_move();
    do
    {
        execute_move(move);
        is_game_over = update_board(board);
        move = get_move(board);
    }
    while(!is_game_over);
    free(board);
}

int main(int argc, char *argv[])
{
    if (argc != ARGS_NUMBER)
    {
        printf(NUMBER_ARGUMENT_MESSAGE, argc);
        exit(1);
    }
    char * input_level = argv[ARG_GAME_LEVEL];
    if (!is_level_valid(input_level))
    {
        printf(INPUT_ERROR_MESSAGE);
        exit(1);
    }
    set_level_global_parameters(input_level);
    start_game();
}
