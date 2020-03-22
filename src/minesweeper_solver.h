#ifndef MINESWEEPERSOLVER_MINESWEEPER_SOLVER_H
#define MINESWEEPERSOLVER_MINESWEEPER_SOLVER_H

#include <windows.h>
#include <stdlib.h>
#include <stdbool.h>


struct t_board_size
{
    unsigned short _x;
    unsigned short _y;
};

#define MINESWEEPER_PATH "C:\\Users\\User\\PycharmProjects\\MinesweeperSolver\\MinesweeperGame.exe"

#define UNKNOWN_CELL 9
#define EMPTY_CELL 0
#define MINE 10

#define BEGINNER_MARK 0
#define INTERMEDIATE_MARK 1
#define EXPERT_MARK 2

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

static t_board_size board_size;
static char game_level;

#endif //MINESWEEPERSOLVER_MINESWEEPER_SOLVER_H
