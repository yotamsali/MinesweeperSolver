#include <stdlib.h>
#include "common.h"
#include "board.h"
#include "board_analyzer.h"
#include "logger.h"
#include "matrix.h"

#define NEIGHBORS_NUMBER 8
#define VARIABLES_MAP_NULL -1.0
#define VARIABLES_MAP_MINE -2.0
#define VARIABLES_MAP_CLEAR -3.0
#define IS_DETERMINISTIC(x) (x == VARIABLES_MAP_MINE || x == VARIABLES_MAP_CLEAR)
#define NEIGHBOR_CELLS(cell) {{cell.x - 1, cell.y - 1},   \
                                {cell.x - 1, cell.y},     \
                                {cell.x - 1, cell.y + 1}, \
                                {cell.x, cell.y - 1},     \
                                {cell.x, cell.y + 1},     \
                                {cell.x + 1, cell.y - 1}, \
                                {cell.x + 1, cell.y},     \
                                {cell.x + 1, cell.y + 1}} \

struct neighbors_data {
    int unknowns;
    int mines;
};

typedef struct neighbors_data t_neighbors_data;

bool is_numeric_cell(t_ptr_board board, t_board_cell cell) {
    t_cell_type cell_value = BOARD_CELL(board, cell);
    return cell_value != MINE && cell_value != UNKNOWN_CELL;
}

float get_isolated_clear_probability(int total_number_of_mines, t_ptr_board board) {
    int detected_mines_counter = 0;
    int unknown_cells_counter = 0;
    for (int i = 0; i < board_size.x; i++)
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            if (BOARD_CELL(board, cell) == MINE)
                detected_mines_counter++;
            if (BOARD_CELL(board, cell) == UNKNOWN_CELL)
                unknown_cells_counter++;
        }
    return 1 - ((float) (total_number_of_mines - detected_mines_counter) /
                (float) (unknown_cells_counter));
}

bool is_cell_in_board(t_board_cell cell) {
    if (cell.x >= board_size.x || cell.x < 0)
        return false;
    if (cell.y >= board_size.y || cell.y < 0)
        return false;
    return true;
}

t_neighbors_data get_neighbors_data(t_ptr_board board, t_board_cell neighbor_cells[]) {
    int unknown_counter = 0, mine_counter = 0;
    for (int k = 0; k < NEIGHBORS_NUMBER; k++)
        if (is_cell_in_board(neighbor_cells[k])) {
            t_cell_type cell_value = BOARD_CELL(board, neighbor_cells[k]);
            if (cell_value == MINE)
                mine_counter++;
            else if (cell_value == UNKNOWN_CELL)
                unknown_counter++;
        }
    t_neighbors_data neighbors_data = {unknown_counter, mine_counter};
    return neighbors_data;
}



t_error_code get_matrix_dimensions(t_ptr_board board, t_matrix_size *matrix_size) {
    matrix_size->x = 0;
    matrix_size->y = 1;
    t_ptr_matrix is_cell_variable_marked = initialize_matrix(board_size, false);
    if (!is_cell_variable_marked)
        return ERROR_INITIALIZE_MATRIX_MEMORY_ALLOC;
    for (int i = 0; i < board_size.x; i++)
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            bool is_cell_equation = false;
            if (!is_numeric_cell(board, cell))
                continue;
            t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
            for (int k = 0; k < NEIGHBORS_NUMBER; k++) {
                if (is_cell_in_board(neighbor_cells[k])
                    && BOARD_CELL(board, neighbor_cells[k]) == UNKNOWN_CELL) {
                    is_cell_equation = true;
                    if (BOARD_CELL(is_cell_variable_marked, neighbor_cells[k]) == false) {
                        BOARD_CELL(is_cell_variable_marked, neighbor_cells[k]) = true;
                        matrix_size->y += 1;
                    }
                }
            }
            if (is_cell_equation)
                matrix_size->x += 1;
        }
    free(is_cell_variable_marked);
    return RETURN_CODE_SUCCESS;
}

t_board_cell extract_board_cell(int variable_number, t_ptr_matrix variables_map) {
    t_board_cell cell = {0, 0};
    for (int i = 0; i < board_size.x; i++) {
        for (int j = 0; j < board_size.y; j++) {
            cell.x = i;
            cell.y = j;
            if ((int)BOARD_CELL(variables_map, cell) == variable_number)
                goto lblReturn;
        }
    }
    lblReturn:
    ASSERT(BOARD_CELL(variables_map, cell) == variable_number);
    return cell;
}

bool is_informative_row(t_ptr_matrix matrix, t_matrix_size matrix_size, int row) {
    for (int col = 0; col < matrix_size.y - 1; col++) {
        t_matrix_cell cell = {row, col};
        if (MATRIX_CELL(matrix, matrix_size, cell) < 0)
            return false;
    }
    return true;
}

int mark_row_variables(t_ptr_matrix variables_map, t_ptr_matrix matrix, t_ptr_matrix deterministic_map,
                                        t_matrix_size matrix_size, int row, bool is_mine) {
    int marked_variables = 0;
    for (int col = 0; col < matrix_size.y - 1; col++) {
        t_matrix_cell cell = {row, col};
        if (MATRIX_CELL(matrix, matrix_size, cell) == 0)
            continue;
        t_board_cell board_cell = extract_board_cell(col, variables_map);
        if (IS_DETERMINISTIC(BOARD_CELL(deterministic_map, board_cell)))
            continue;
        if (is_mine)
            BOARD_CELL(deterministic_map, board_cell) = VARIABLES_MAP_MINE;
        else
            BOARD_CELL(deterministic_map, board_cell) = VARIABLES_MAP_CLEAR;
        marked_variables++;
    }
    return marked_variables;
}

int mark_deterministic_cells(t_ptr_matrix matrix, t_ptr_matrix variables_map,
                            t_ptr_matrix deterministic_map, t_matrix_size matrix_size) {
    int deterministic_cells = 0;
    for (int row = 0; row < matrix_size.x; row++) {
        double row_sum = 0, bias = 0;
        if (!is_informative_row(matrix, matrix_size, row))
            continue;
        for (int col = 0; col < matrix_size.y - 1; col++) {
            t_matrix_cell cell = {row, col};
            row_sum += MATRIX_CELL(matrix, matrix_size, cell);
        }
        t_matrix_cell bias_cell = {row, matrix_size.y - 1};
        bias = MATRIX_CELL(matrix, matrix_size, bias_cell);
        if (row_sum == 0)
            continue;
        ASSERT(row_sum > 0)
        if (bias == 0)
            deterministic_cells += mark_row_variables(variables_map, matrix, deterministic_map, matrix_size, row, false);
        else if (bias == row_sum)
            deterministic_cells += mark_row_variables(variables_map, matrix, deterministic_map, matrix_size, row, true);
    }
    return deterministic_cells;
}

void extract_deterministic_moves(t_ptr_matrix deterministic_map, int number_of_deterministic_cells, t_moves *moves) {
    moves->number_of_moves = number_of_deterministic_cells;
    moves->moves = (t_move *) malloc(sizeof(t_move) * number_of_deterministic_cells);
    int current_move = 0;
    for (int i = 0; i < board_size.x; i++)
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            if (IS_DETERMINISTIC(BOARD_CELL(deterministic_map, cell))) {
                moves->moves[current_move].cell = cell;
                moves->moves[current_move].is_mine = (BOARD_CELL(deterministic_map, cell) == VARIABLES_MAP_MINE);
                current_move++;
            }
        }
}

t_ptr_matrix fill_matrix(t_ptr_board board, t_ptr_matrix matrix, t_matrix_size matrix_size) {
    int variables_counter = 0;
    int current_equation = 0;
    t_ptr_matrix variables_map = initialize_matrix(board_size, VARIABLES_MAP_NULL);
    for (int i = 0; i < board_size.x; i++) {
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            if (!is_numeric_cell(board, cell))
                continue;
            t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
            t_neighbors_data neighbors_data = get_neighbors_data(board, neighbor_cells);
            if (neighbors_data.unknowns == 0)
                continue;
            for (int k = 0; k < NEIGHBORS_NUMBER; k++) {
                if (is_cell_in_board(neighbor_cells[k]) && BOARD_CELL(board, neighbor_cells[k]) == UNKNOWN_CELL) {
                    if (BOARD_CELL(variables_map, neighbor_cells[k]) == VARIABLES_MAP_NULL) {
                        BOARD_CELL(variables_map, neighbor_cells[k]) = variables_counter;
                        variables_counter++;
                    }
                    t_board_cell matrix_cell = {current_equation, BOARD_CELL(variables_map, neighbor_cells[k])};
                    MATRIX_CELL(matrix, matrix_size, matrix_cell) = 1;
                }
            }
            t_board_cell bias_cell = {current_equation, matrix_size.y - 1};
            MATRIX_CELL(matrix, matrix_size, bias_cell) = BOARD_CELL(board, cell) - neighbors_data.mines;
            current_equation++;
        }
    }
    return variables_map;
}

bool is_legal_board(t_ptr_board board) {
    for (int i = 0; i < board_size.x; i++)
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            if (is_numeric_cell(board, cell)) {
                t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
                t_neighbors_data neighbors_data = get_neighbors_data(board, neighbor_cells);
                int cell_number = BOARD_CELL(board, cell);
                if (neighbors_data.mines > cell_number || cell_number > neighbors_data.mines + neighbors_data.unknowns) {
                    log_illegal_cell(cell);
                    return false;
                }
            }
        }
    return true;
}

void update_board_by_moves(t_ptr_board board, t_moves moves) {
    for (int i = 0; i < moves.number_of_moves; i++) {
        t_move move = moves.moves[i];
        if (move.is_mine)
            BOARD_CELL(board, move.cell) = MINE;
    }
}

t_board_cell get_random_isolated_cell(t_ptr_board board, t_ptr_matrix variables_map) {
    t_board_cell random_isolated_cell = {0, 0};
    int isolated_cells_number = 0;
    int random_isolated_index = 0;
    int index_counter = 0;
    for (int i = 0; i < board_size.x; i++)
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            if (BOARD_CELL(board, cell) == UNKNOWN_CELL && BOARD_CELL(variables_map, cell) == VARIABLES_MAP_NULL)
                isolated_cells_number++;
        }
    random_isolated_index = (rand() % isolated_cells_number);
    for (int i = 0; i < board_size.x; i++)
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            if (BOARD_CELL(board, cell) == UNKNOWN_CELL && BOARD_CELL(variables_map, cell) == VARIABLES_MAP_NULL) {
                if (index_counter == random_isolated_index) {
                    random_isolated_cell = cell;
                    goto lblReturn;
                }
                index_counter++;
            }
        }
    lblReturn:
    return random_isolated_cell;
}

float get_best_clear_variable(t_board_cell *best_variable_clear, t_ptr_matrix matrix,
                              t_matrix_size matrix_size, t_ptr_matrix variables_map) {
    float maximal_clear_probability = 0;
    for (int row = 0; row < matrix_size.x; row++) {
        double row_sum = 0;
        for (int col = 0; col < matrix_size.y - 1; col++) {
            t_matrix_cell cell = {row, col};
            row_sum += MATRIX_CELL(matrix, matrix_size, cell);
        }
        t_matrix_cell bias_cell = {row, matrix_size.y - 1};
        if (row_sum == 0 || !is_informative_row(matrix, matrix_size, row))
            continue;
        float equation_clear_probability = 1 - (MATRIX_CELL(matrix, matrix_size, bias_cell) / row_sum);
        if (equation_clear_probability > maximal_clear_probability) {
            maximal_clear_probability = equation_clear_probability;
            int random_variable_index = (rand() % (int)row_sum);
            int variable_counter = 0;
            for (int col = 0; col < matrix_size.y - 1; col++) {
                t_matrix_cell cell = {row, col};
                if (MATRIX_CELL(matrix, matrix_size, cell) != 0) {
                    if (random_variable_index == variable_counter) {
                        *best_variable_clear = extract_board_cell(col, variables_map);
                        goto lblReturn;
                    }
                    variable_counter++;
                }
            }
        }
    }
    lblReturn:
    return maximal_clear_probability;
}

void make_best_guess(t_ptr_board board, t_moves *moves, t_ptr_matrix variables_map,
                t_ptr_matrix matrix, t_matrix_size matrix_size, int total_number_of_mines) {
    t_board_cell best_variable_cell = {0, 0};
    t_move *bet_clear_move = (t_move *) malloc(sizeof(t_move));
    float isolated_clear_probability = get_isolated_clear_probability(total_number_of_mines, board);
    float variable_clear_probability = get_best_clear_variable(&best_variable_cell, matrix, matrix_size, variables_map);
    if (variable_clear_probability >= isolated_clear_probability)
        bet_clear_move->cell = best_variable_cell;
    else
        bet_clear_move->cell = get_random_isolated_cell(board, variables_map);
    bet_clear_move->is_mine = false;
    moves->moves = bet_clear_move;
    moves->number_of_moves = 1;
}

t_error_code get_moves(t_ptr_board board, t_moves *moves, int total_number_of_mines) {
    if (!is_legal_board(board))
        return ERROR_GET_MOVE_ILLEGAL_BOARD_DETECTED;
    t_matrix_size matrix_size = {0, 0};
    t_error_code error_code = get_matrix_dimensions(board, &matrix_size);
    if (error_code)
        return error_code;
    t_ptr_matrix matrix = initialize_matrix(matrix_size, 0);
    t_ptr_matrix variables_map = fill_matrix(board, matrix, matrix_size);
    if (!matrix || !variables_map)
        return ERROR_INITIALIZE_MATRIX_MEMORY_ALLOC;
    log_variables_map(variables_map);
    gauss_eliminate(matrix, matrix_size);
    t_ptr_matrix deterministic_map = initialize_matrix(board_size, VARIABLES_MAP_NULL);
    if (!deterministic_map)
        return ERROR_INITIALIZE_MATRIX_MEMORY_ALLOC;
    int deterministic_cells = mark_deterministic_cells(matrix, variables_map, deterministic_map, matrix_size);
    if (deterministic_cells > 0)
        extract_deterministic_moves(deterministic_map, deterministic_cells, moves);
    else {
        make_best_guess(board, moves, variables_map, matrix, matrix_size, total_number_of_mines);
    }
    free(variables_map);
    free(deterministic_map);
    free(matrix);
    error_code = log_moves(*moves);
    if (error_code)
        return error_code;
    update_board_by_moves(board, *moves);
    return RETURN_CODE_SUCCESS;
}
