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
#define NEIGHBOR_CELLS(cell) {{cell.row - 1, cell.col - 1},   \
                                {cell.row - 1, cell.col},     \
                                {cell.row - 1, cell.col + 1}, \
                                {cell.row, cell.col - 1},     \
                                {cell.row, cell.col + 1},     \
                                {cell.row + 1, cell.col - 1}, \
                                {cell.row + 1, cell.col},     \
                                {cell.row + 1, cell.col + 1}} \

struct neighbors_data {
    int unknowns;
    int mines;
};

typedef struct neighbors_data t_neighbors_data;

bool is_numeric_cell(t_board board, t_board_cell cell) {
    t_cell_type cell_value = BOARD_CELL(board, cell.row, cell.col);
    return cell_value != MINE && cell_value != UNKNOWN_CELL;
}

float get_isolated_clear_probability(int total_number_of_mines, t_board board) {
    int detected_mines_counter = 0;
    int unknown_cells_counter = 0;
    for (int row = 0; row < board_size.rows; row++)
        for (int col = 0; col < board_size.cols; col++) {
            if (BOARD_CELL(board, row, col) == MINE)
                detected_mines_counter++;
            if (BOARD_CELL(board, row, col) == UNKNOWN_CELL)
                unknown_cells_counter++;
        }
    return 1 - ((float) (total_number_of_mines - detected_mines_counter) /
                (float) (unknown_cells_counter));
}

bool is_cell_in_board(t_board_cell cell) {
    if (cell.row >= board_size.rows || cell.row < 0)
        return false;
    if (cell.col >= board_size.cols || cell.col < 0)
        return false;
    return true;
}

t_neighbors_data get_neighbors_data(t_board board, t_board_cell neighbor_cells[]) {
    int unknown_counter = 0, mine_counter = 0;
    for (int k = 0; k < NEIGHBORS_NUMBER; k++)
        if (is_cell_in_board(neighbor_cells[k])) {
            t_cell_type cell_value = BOARD_CELL(board, neighbor_cells[k].row, neighbor_cells[k].col);
            if (cell_value == MINE)
                mine_counter++;
            else if (cell_value == UNKNOWN_CELL)
                unknown_counter++;
        }
    t_neighbors_data neighbors_data = {unknown_counter, mine_counter};
    return neighbors_data;
}


t_error_code get_matrix_dimensions(t_board board, t_matrix_size *matrix_size) {
    matrix_size->rows = 0;
    matrix_size->cols = 1;
    t_matrix is_cell_variable_marked = initialize_matrix(board_size, false);
    if (!is_cell_variable_marked.data)
        return ERROR_INITIALIZE_MATRIX_MEMORY_ALLOC;
    for (int row = 0; row < board_size.rows; row++)
        for (int col = 0; col < board_size.cols; col++) {
            t_board_cell cell = {row, col};
            bool is_cell_equation = false;
            if (!is_numeric_cell(board, cell))
                continue;
            t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
            for (int k = 0; k < NEIGHBORS_NUMBER; k++) {
                if (is_cell_in_board(neighbor_cells[k])
                    && BOARD_CELL(board, neighbor_cells[k].row, neighbor_cells[k].col) == UNKNOWN_CELL) {
                    is_cell_equation = true;
                    if (MATRIX_CELL(is_cell_variable_marked, neighbor_cells[k].row, neighbor_cells[k].col) == false) {
                        MATRIX_CELL(is_cell_variable_marked, neighbor_cells[k].row, neighbor_cells[k].col) = true;
                        matrix_size->cols += 1;
                    }
                }
            }
            if (is_cell_equation)
                matrix_size->rows += 1;
        }
    free(is_cell_variable_marked.data);
    return RETURN_CODE_SUCCESS;
}

t_board_cell extract_board_cell(int variable_number, t_matrix variables_map) {
    t_board_cell cell = {0, 0};
    for (int row = 0; row < board_size.rows; row++) {
        for (int col = 0; col < board_size.cols; col++) {
            if ((int) MATRIX_CELL(variables_map, row, col) == variable_number) {
                cell.row = row;
                cell.col = col;
                goto lblReturn;
            }
        }
    }
    lblReturn:
    ASSERT(BOARD_CELL(variables_map, cell.row, cell.col) == variable_number);
    return cell;
}

bool is_informative_row(t_matrix matrix, int row) {
    for (int col = 0; col < matrix.size.cols - 1; col++) {
        if (MATRIX_CELL(matrix, row, col) < 0)
            return false;
    }
    return true;
}

void zero_variable_in_all_next_equations(t_matrix matrix, int col, int row, double variable_value) {
    for (int equation = row - 1; equation >= 0; equation--) {
        if (MATRIX_CELL(matrix, equation, col) != 0) {
            if (variable_value != 0)
                MATRIX_CELL(matrix, equation, matrix.size.cols - 1) -= MATRIX_CELL(matrix, equation, col);
            MATRIX_CELL(matrix, equation, col) = 0;
        }
    }
}

int extract_partial_solution_from_row(t_matrix variables_map, t_matrix matrix, t_matrix deterministic_map,
                       int row, bool is_upper_bound) {
    int marked_variables = 0;
    for (int col = 0; col < matrix.size.cols - 1; col++) {
        t_board_cell board_cell = extract_board_cell(col, variables_map);
        if (IS_DETERMINISTIC(MATRIX_CELL(deterministic_map, board_cell.row, board_cell.col)))
            continue;
        bool is_mine = (MATRIX_CELL(matrix, row, col) > 0 && is_upper_bound) || (MATRIX_CELL(matrix, row, col) < 0 && !is_upper_bound);
        bool is_clear = (MATRIX_CELL(matrix, row, col) > 0 && !is_upper_bound) || (MATRIX_CELL(matrix, row, col) < 0 && is_upper_bound);
        if (is_mine) {
            MATRIX_CELL(deterministic_map, board_cell.row, board_cell.col) = VARIABLES_MAP_MINE;
            zero_variable_in_all_next_equations(matrix, col, row, 1);
        }
        else if (is_clear) {
            MATRIX_CELL(deterministic_map, board_cell.row, board_cell.col) = VARIABLES_MAP_CLEAR;
            zero_variable_in_all_next_equations(matrix, col, row, 0);
        }
        if (is_mine || is_clear)
            marked_variables++;
    }
    return marked_variables;
}

int mark_deterministic_cells(t_matrix matrix, t_matrix variables_map,
                             t_matrix deterministic_map) {
    int deterministic_cells = 0;
    int last_non_zero_row = get_last_non_zero_row(matrix);
    for (int row = last_non_zero_row; row < matrix.size.rows && row >= 0; row--) {
        double row_upper_bound = get_row_upper_bound(matrix, row);
        double row_lower_bound = get_row_lower_bound(matrix, row);
        double row_bias = MATRIX_CELL(matrix, row, matrix.size.cols - 1);
        if (row_bias == row_lower_bound)
            deterministic_cells += extract_partial_solution_from_row(variables_map, matrix, deterministic_map, row, false);
        else if (row_bias == row_upper_bound) {
            deterministic_cells += extract_partial_solution_from_row(variables_map, matrix, deterministic_map, row, true);
        }
    }
    return deterministic_cells;
}

void extract_deterministic_moves(t_matrix deterministic_map, int number_of_deterministic_cells, t_moves *moves) {
    moves->number_of_moves = number_of_deterministic_cells;
    moves->moves = (t_move *) malloc(sizeof(t_move) * number_of_deterministic_cells);
    int current_move = 0;
    for (int row = 0; row < board_size.rows; row++)
        for (int col = 0; col < board_size.cols; col++) {
            t_board_cell cell = {row, col};
            if (IS_DETERMINISTIC(MATRIX_CELL(deterministic_map, row, col))) {
                moves->moves[current_move].cell = cell;
                moves->moves[current_move].is_mine = (MATRIX_CELL(deterministic_map, row, col) == VARIABLES_MAP_MINE);
                current_move++;
            }
        }
}

t_matrix fill_matrix(t_board board, t_matrix matrix) {
    int variables_counter = 0;
    int current_equation = 0;
    t_matrix variables_map = initialize_matrix(board_size, VARIABLES_MAP_NULL);
    for (int row = 0; row < board_size.rows; row++) {
        for (int col = 0; col < board_size.cols; col++) {
            t_board_cell cell = {row, col};
            if (!is_numeric_cell(board, cell))
                continue;
            t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
            t_neighbors_data neighbors_data = get_neighbors_data(board, neighbor_cells);
            if (neighbors_data.unknowns == 0)
                continue;
            for (int k = 0; k < NEIGHBORS_NUMBER; k++) {
                if (is_cell_in_board(neighbor_cells[k]) &&
                    BOARD_CELL(board, neighbor_cells[k].row, neighbor_cells[k].col) == UNKNOWN_CELL) {
                    if (MATRIX_CELL(variables_map, neighbor_cells[k].row, neighbor_cells[k].col) == VARIABLES_MAP_NULL) {
                        MATRIX_CELL(variables_map, neighbor_cells[k].row, neighbor_cells[k].col) = variables_counter;
                        variables_counter++;
                    }
                    MATRIX_CELL(matrix, current_equation,
                                (int) MATRIX_CELL(variables_map, neighbor_cells[k].row, neighbor_cells[k].col)) = 1;
                }
            }
            MATRIX_CELL(matrix, current_equation, matrix.size.cols - 1) = BOARD_CELL(board, row, col) - neighbors_data.mines;
            current_equation++;
        }
    }
    return variables_map;
}

bool is_legal_board(t_board board) {
    for (int row = 0; row < board_size.rows; row++)
        for (int col = 0; col < board_size.cols; col++) {
            t_board_cell cell = {row, col};
            if (is_numeric_cell(board, cell)) {
                t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
                t_neighbors_data neighbors_data = get_neighbors_data(board, neighbor_cells);
                int cell_number = BOARD_CELL(board, row, col);
                if (neighbors_data.mines > cell_number ||
                    cell_number > neighbors_data.mines + neighbors_data.unknowns) {
                    log_illegal_cell(cell);
                    return false;
                }
            }
        }
    return true;
}

void update_board_by_moves(t_board board, t_moves moves) {
    for (int i = 0; i < moves.number_of_moves; i++) {
        t_move move = moves.moves[i];
        if (move.is_mine)
            BOARD_CELL(board, move.cell.row, move.cell.col) = MINE;
    }
}

t_board_cell get_random_isolated_cell(t_board board, t_matrix variables_map) {
    t_board_cell random_isolated_cell = {0, 0};
    int isolated_cells_number = 0;
    int random_isolated_index = 0;
    int index_counter = 0;
    for (int row = 0; row < board_size.rows; row++)
        for (int col = 0; col < board_size.cols; col++) {
            if (BOARD_CELL(board, row, col) == UNKNOWN_CELL && MATRIX_CELL(variables_map, row, col) == VARIABLES_MAP_NULL)
                isolated_cells_number++;
        }
    random_isolated_index = (rand() % isolated_cells_number);
    for (int row = 0; row < board_size.rows; row++)
        for (int col = 0; col < board_size.cols; col++) {
            if (BOARD_CELL(board, row, col) == UNKNOWN_CELL && MATRIX_CELL(variables_map, row, col) == VARIABLES_MAP_NULL) {
                if (index_counter == random_isolated_index) {
                    random_isolated_cell.row = row;
                    random_isolated_cell.col = col;
                    goto lblReturn;
                }
                index_counter++;
            }
        }
    lblReturn:
    return random_isolated_cell;
}

//TODO: Update according to max / min range (with relative part).
float get_best_clear_variable(t_board_cell *best_variable_clear, t_matrix matrix, t_matrix variables_map) {
    float maximal_clear_probability = 0;
    for (int row = 0; row < matrix.size.rows; row++) {
        double row_sum = 0;
        for (int col = 0; col < matrix.size.cols - 1; col++) {
            row_sum += MATRIX_CELL(matrix, row, col);
        }
        if (row_sum == 0 || !is_informative_row(matrix, row))
            continue;
        float equation_clear_probability = 1 - (MATRIX_CELL(matrix, row, matrix.size.cols - 1) / row_sum);
        if (equation_clear_probability > maximal_clear_probability) {
            maximal_clear_probability = equation_clear_probability;
            int random_variable_index = (rand() % (int) row_sum);
            int variable_counter = 0;
            for (int col = 0; col < matrix.size.cols - 1; col++) {
                if (MATRIX_CELL(matrix, row, col) != 0) {
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

void make_best_guess(t_board board, t_moves *moves, t_matrix variables_map,
                     t_matrix matrix, int total_number_of_mines) {
    t_board_cell best_variable_cell = {0, 0};
    t_move *bet_clear_move = (t_move *) malloc(sizeof(t_move));
    float isolated_clear_probability = get_isolated_clear_probability(total_number_of_mines, board);
    float variable_clear_probability = get_best_clear_variable(&best_variable_cell, matrix, variables_map);
    if (variable_clear_probability >= isolated_clear_probability)
        bet_clear_move->cell = best_variable_cell;
    else
        bet_clear_move->cell = get_random_isolated_cell(board, variables_map);
    bet_clear_move->is_mine = false;
    moves->moves = bet_clear_move;
    moves->number_of_moves = 1;
}

t_error_code get_moves(t_board board, t_moves *moves, int total_number_of_mines) {
    if (!is_legal_board(board))
        return ERROR_GET_MOVE_ILLEGAL_BOARD_DETECTED;
    t_matrix_size matrix_size = {0, 0};
    t_error_code error_code = get_matrix_dimensions(board, &matrix_size);
    if (error_code)
        return error_code;
    t_matrix matrix = initialize_matrix(matrix_size, 0);
    t_matrix variables_map = fill_matrix(board, matrix);
    if (!matrix.data || !variables_map.data)
        return ERROR_INITIALIZE_MATRIX_MEMORY_ALLOC;
    log_variables_map(variables_map);
    gauss_eliminate(matrix);
    t_matrix deterministic_map = initialize_matrix(board_size, VARIABLES_MAP_NULL);
    if (!deterministic_map.data)
        return ERROR_INITIALIZE_MATRIX_MEMORY_ALLOC;
    int deterministic_cells = mark_deterministic_cells(matrix, variables_map, deterministic_map);
    if (deterministic_cells > 0)
        extract_deterministic_moves(deterministic_map, deterministic_cells, moves);
    else {
        make_best_guess(board, moves, variables_map, matrix, total_number_of_mines);
    }
    free(variables_map.data);
    free(deterministic_map.data);
    free(matrix.data);
    error_code = log_moves(*moves);
    if (error_code)
        return error_code;
    update_board_by_moves(board, *moves);
    return RETURN_CODE_SUCCESS;
}
