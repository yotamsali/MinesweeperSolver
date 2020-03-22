#include "minesweeper_solver_utils.h"
#include "commander.h"
#include "board.h"
#include "board_analyzer.h"

int main(int argc, char *argv[])
{
    if(!handle_input(argc, argv))
    {
        exit(1);
    }
    raise_app();
    t_ptr_board board = initialize_board_ptr(board_size);
    t_move move = get_first_move();
    do
    {
        execute_move(move);
        get_board(board);
        move = get_move(board);
    }
    while(!is_game_over(board));
    free(board);
}
