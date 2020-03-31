#include "minesweeper_solver_utils.h"
#include "commander.h"
#include "board.h"
#include "board_analyzer.h"
#include "minesweeper_solver.h"

void start_game()
{
    // CR: An uninitialized variable?! Are you insane?
    bool is_game_over;
    // CR: Which app? Is it an argument? If not, it's probably a specific app.
    // CR: Rename to raise_minesweeper.
    raise_app();
    set_level_in_app();
    t_ptr_board board = initialize_board_ptr(board_size);
    t_move move = get_first_move();
    while(true)
    {
        execute_move(move);
        // CR: Can't is_game_over be defined here, in the loop?
        // CR: Are you afraid of the time penalty of redefining a variable over and over again?
        // CR: If so, don't. The compiler is smarter than you, it's smarter than me, it's smarter than us all.
        // CR: Together. Times infinity. Plus one.
        is_game_over = update_board(board);
        if (is_game_over)
            break;
        move = get_move(board);
    }
    free(board);
}

int main(int argc, char *argv[])
{
    // CR: At the beginning of a function, we must check its parameters. The main() function gets the
    // CR: parameters from libc (specifically __libc_start_main, if you want to read online).
    // CR: Thus, we can be sure that the parameters are valid, as __libc_start_main is a library function.
    // CR: In case we are sure the parameters are valid, we still check them, but in a lightweight manner,
    // CR: using ASSERT. For example, ASSERT(NULL != argv).
    // CR: Create your own macro ASSERT() that will evaluate an expression assert it's true. If the expression
    // CR: evaluates to false, the macro will print an error message stating the file, the line and the
    // CR: expression that caused the failure. Read online how to attain all this information in C macros.
    // CR: Remember, macros are powerful. You will be surprised what the # symbol can do in macros.
    // CR: Additionally, create the ASSERT macro so that it will only run in DEBUG mode. In Release mode,
    // CR: it should simply resolve to nothing.
    // CR: The ASSERT macro is very common, and can be used to assert other invariants you assume
    // CR: in your code, only in Debug mode, for debugging purposes.

    if (argc != ARGS_NUMBER)
    {
        // CR: I personally would stick the message right here as a string, and not use a #define for it,
        // CR: but it's just me. Think about what you prefer, if you'd rather have a #define, so be it.
        // CR: Nonetheless, the error message is not indicative, as it doesn't tell the user what's the
        // CR: correct way to invoke the executable.
        // CR: Add a "Usage" message, a-la Python's argparse module (of course you don't have to reach
        // CR: it's level of sophistication, we're writing in C after all).
        printf(NUMBER_ARGUMENT_MESSAGE, argc);
        // CR: Calls to exit() are bad practice, even though here they have no real negative impact.
        // CR: Change it to a return or a goto lblCleanup.
        exit(1);
    }
    // CR: Is this variable necessary? Can't you use argv[ARG_GAME_LEVEL] both times?
    // CR: Don't worry about time optimizations. Your compiler is smarter than you think, and probably
    // CR: smarter than all of us together.
    char * input_level = argv[ARG_GAME_LEVEL];
    if (!is_level_valid(input_level))
    {
        // CR: As stated above, I would not use a #define and stick the message right here, but it's a
        // CR: matter of style and personal preference, so choose what works best for you.
        printf(INPUT_ERROR_MESSAGE);
        // CR: Like I stated above, calls to exit() are evil. They truly are.
        exit(1);
    }
    set_level_global_parameters(input_level);
    start_game();
}
