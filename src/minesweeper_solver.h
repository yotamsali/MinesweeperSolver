#ifndef MINESWEEPERSOLVER_MINESWEEPER_SOLVER_H
#define MINESWEEPERSOLVER_MINESWEEPER_SOLVER_H

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct t_board_size
{
    unsigned short _x;
    unsigned short _y;
};

#define MINESWEEPER_PATH "C:\\Users\\User\\PycharmProjects\\MinesweeperSolver\\MinesweeperGame\\Minesweeper X.exe"
#define MINESWEEPER_WINDOW_NAME "Minesweeper X"

#define UNKNOWN_CELL 9
#define EMPTY_CELL 0
#define MINE 10

#define BEGINNER_MARK 0
#define INTERMEDIATE_MARK 1
#define EXPERT_MARK 2

#define ARGS_NUMBER 2
#define ARG_GAME_LEVEL 1

#define INPUT_ERROR_MESSAGE "Input must be 'beginner', 'intermediate' or 'expert'."
#define NUMBER_ARGUMENT_MESSAGE "Number of arguments cannot be %d."

#define GET_CELL(board, cell) board[cell._x * board_size._x + cell._y]
#define SET_CELL(board, cell, value) board[cell._x * board_size._x + cell._y] = value;


typedef struct t_board_size t_board_size;
typedef struct t_board_size t_board_cell;
typedef unsigned short * t_ptr_board;
typedef float * t_ptr_map;

struct t_move
{
    t_board_cell _cell;
    bool _is_mine;
};
typedef struct t_move t_move;

// Globals
t_board_size board_size;
extern t_board_size board_size;
char game_level_mark;
extern char game_level_mark;

void start_game();

#endif //MINESWEEPERSOLVER_MINESWEEPER_SOLVER_H
