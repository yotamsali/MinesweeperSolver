#include "minesweeper_solver_utils.h"
#include "minesweeper_solver.h"

const t_board_size EXPERT_BOARD_SIZE = {16, 30};
const t_board_size INTERMEDIATE_BOARD_SIZE = {16, 16};
const t_board_size BEGINNER_BOARD_SIZE = {8, 8};

void set_level_global_parameters(const char * level)
{
    // CR: I tend to dislike this if-chain approach. It has several disadvantages:
    // CR:  1. If I would like to add a level, I must remember to add that level here.
    // CR:  2. I must remember to add that level in is_level_valid.
    // CR:  3. The level_MARK and level_BOARD_SIZE are too far apart.
    // CR:  4. The level_BOARD_LEVEL constant is also far apart from level_MARK and level_BOARD_SIZE.
    // CR: I suggest a different approach. I suggest keeping a global array of available levels:
    // CR: t_level g_levels[NUM_LEVELS];
    // CR: Each cell in this array will hold a t_level, which is a struct describing the level, containing
    // CR: all information related to that level in a single place. This way it's easy to find all level-related
    // CR: properties, as they reside in the struct describing that level.
    // CR: Plus, you don't need the level_MARK constants anymore, as the index of the level in the array
    // CR: can be its mark!
    // CR: Another advantage is that adding a new level becomes an easy task, you just add another entry
    // CR: in g_levels.
    // CR: Afterwards, you can ditch all the long if-chains and use for-loops to work with t_levels.
    // CR: This approach is more Python-like and OOP-like than classic procedural C.
    if (strcmp(level, EXPERT_BOARD_LEVEL) == 0)
    {
        board_size = EXPERT_BOARD_SIZE;
        game_level_mark = EXPERT_MARK;
    }
    else if (strcmp(level, INTERMEDIATE_BOARD_LEVEL) == 0)
    {
        board_size = INTERMEDIATE_BOARD_SIZE;
        game_level_mark = INTERMEDIATE_MARK;
    }
    else
    {
        // CR: You didn't check that level == 'beginner' here, because your code calls is_level_valid
        // CR: before and it verifies that the level is one of three values.
        // CR: This is OK, but this is just the place to use an ASSERT (like I explained in main()).
        // CR: Here, you can ASSERT that level truly is equal to 'beginner'.
        board_size = BEGINNER_BOARD_SIZE;
        game_level_mark = INTERMEDIATE_MARK;
    }
}

bool is_level_valid(const char * level)
{
    // CR: strcmp is considered unsafe. If the string passed isn't null-terminated, strcmp
    // CR: will keep rolling over it forever, or until the OS will shut you down.
    // CR: A safer alternative is to use strncmp.
    // CR: Change that everywhere you use strcmp.
    return  ((strcmp(level, EXPERT_BOARD_LEVEL) == 0) ||
            (strcmp(level, INTERMEDIATE_BOARD_LEVEL) == 0) ||
            (strcmp(level,BEGINNER_BOARD_LEVEL) == 0));
}

void set_board_to_unknown(t_ptr_board board)
{
    int board_cells_number = board_size._x * board_size._y;
    for (int i = 0; i < board_cells_number; i++)
        board[i] = UNKNOWN_CELL;
}

t_ptr_board initialize_board_ptr()
{
    unsigned short board_memory_size = board_size._x * board_size._y * sizeof(unsigned short);
    t_ptr_board ptr_board = (t_ptr_board) malloc(board_memory_size);
    set_board_to_unknown(ptr_board);
    return ptr_board;
}

bool is_board_known(t_ptr_board board)
{
    for (int i = 0; i < board_size._x; i++)
        for (int j = 0; j < board_size._y; j++)
        {
            t_board_cell cell = {i, j};
            if (GET_CELL(board, cell) == UNKNOWN_CELL)
            {
                return false;
            }
        }
    return true;
}

t_move get_first_move()
{
    t_move first_move;
    t_board_cell first_move_cell = {board_size._x / 2, board_size._y / 2};
    first_move._is_mine = false;
    first_move._cell = first_move_cell;
    return first_move;
}



