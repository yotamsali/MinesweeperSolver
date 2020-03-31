#ifndef MINESWEEPERSOLVER_MINESWEEPER_SOLVER_H
#define MINESWEEPERSOLVER_MINESWEEPER_SOLVER_H

// CR: A header file such as this is used for a module to declare what is exports to other modules,
// CR: so that the other modules may #include this header file and use the exported symbols.
// CR: You used the header file simply to split definitions and implementation.
// CR: For a module, we use three different files, each for a certain purpose:
// CR:  * module.h defines the symbols the module wants to expose, its API.
// CR:  * module.c holds the implementation of the public functions, private functions,
// CR:    and anything else needed to implement them.
// CR:  * module_internal.h lies somewhere in between module.c and module.h. It is a header file,
// CR:    but it's not intended to be #included by other modules (hence the _internal suffix).
// CR:    Sometimes, when writing UT we need to access internal parts of a module. Technically, to access
// CR:    them, we need them to be defined in a header file, but of course they aren't defined in a header
// CR:    file as they are internal. There comes module_internal.h to the rescue. It contains definitions
// CR:    of internal implementation-specific symbols that are needed for UT purposes.
// CR:    As a side effect of that usage, the module_internal.h file can be also used to split definitions
// CR:    from the module.c file, for readability purposes. This is often used if the module.c file is
// CR:    extremely long and it hinders readability.
// CR: In your case, you use this header file as if it was an _internal.h header file, as this module
// CR: has nothing to export (it is the main() function).
// CR: The .c file of this module is not so long. Hence, I recommend putting all this content in the .c file.
// CR: You may come across several symbols here that can't be moved to the .c file, as they are used from other
// CR: modules as well, for example, board_size. This is a hint that board_size doesn't belong here.
// CR: It probably belongs to board.h, don't you think?

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct board_size
{
    unsigned short _x;
    unsigned short _y;
};

// CR: The difference between these constants and UNKNOWN_CELL, for example, is that UNKNOWN_CELL
// CR: is an implementation-specific constant, that the user of the program should never know about.
// CR: It's simply a marker value for you to use in your implementation.
// CR: MINESWEEPER_PATH, on the other hand, is a value that the user might really be interested in, as they
// CR: might want to change that and recompile your code.
// CR: We put all these user-relevant constants, that the user might want to change, in a separate header file,
// CR: commonly named hard_coded_config.h.
#define MINESWEEPER_PATH "C:\\Users\\User\\PycharmProjects\\MinesweeperSolver\\MinesweeperGame\\Minesweeper X.exe"
#define MINESWEEPER_WINDOW_NAME "Minesweeper X"

#define UNKNOWN_CELL 9
#define EMPTY_CELL 0
#define MINE 10

// CR: Symbol is unused
#define BEGINNER_MARK 0
#define INTERMEDIATE_MARK 1
#define EXPERT_MARK 2

// CR: You can define the arguments like this:
// CR: typedef enum {
// CR:     ARG_EXE_NAME = 0,
// CR:     ARG_GAME_LEVEL,
// CR:
// CR:     // Must be last!
// CR:     ARG_NUMBER
// CR: } t_arg;
// CR: This way, if you add another argument in the future, you won't need to manually
// CR: change both ARGS_NUMBER and the new argument. You can simply add it to the enum,
// CR: and it will work like a charm. Besides, it keeps the arguments and their ARG_NUMBER
// CR: organized.
// CR: It's better to address an enum in a singular tense, hence I named it t_arg.
// CR: And the enum constants should carry the enum's name, hence they start with ARG_
// CR: In addition, it's recommended to add a newline and a comment just before ARG_NUMBER,
// CR: to differentiate it from the rest of the enum, as it doesn't hold an actual argument,
// CR: but it holds the number of enum entries. Thus, it also has to be the last value,
// CR: thus the comment reminding the next developer to keep it that way.
#define ARGS_NUMBER 2
#define ARG_GAME_LEVEL 1

#define INPUT_ERROR_MESSAGE "Input must be 'beginner', 'intermediate' or 'expert'."
#define NUMBER_ARGUMENT_MESSAGE "Number of arguments cannot be %d."

#define GET_CELL(board, cell) board[cell._x * board_size._x + cell._y]
#define SET_CELL(board, cell, value) board[cell._x * board_size._x + cell._y] = value;


typedef struct board_size t_board_size;
typedef struct board_size t_board_cell;
typedef unsigned short * t_ptr_board;
typedef float * t_ptr_map;

struct move
{
    t_board_cell _cell;
    bool _is_mine;
};
typedef struct move t_move;

// CR: Globals shouldn't be in headers, ever.
// CR: Read https://stackoverflow.com/questions/8108634/global-variables-in-header-file
// Globals
t_board_size board_size;
extern t_board_size board_size;
char game_level_mark;
extern char game_level_mark;

// CR: This function is only used from within the module, why is it exported outside?
void start_game();

#endif //MINESWEEPERSOLVER_MINESWEEPER_SOLVER_H
