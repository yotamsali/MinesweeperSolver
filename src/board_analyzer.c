#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "common.h"
#include "board.h"
#include "board_analyzer.h"
#include "logger.h"

#define NEIGHBORS_NUMBER 8
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
    t_cell_type cell_value = GET_CELL(board, cell);
    return cell_value != MINE && cell_value != UNKNOWN_CELL;
}

float get_general_clear_probability(int total_number_of_mines, t_ptr_board board) {
    int detected_mines_counter = 0;
    int unknown_cells_counter = 0;
    for (int i = 0; i < board_size.x; i++)
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            if (GET_CELL(board, cell) == MINE)
                detected_mines_counter++;
            if (GET_CELL(board, cell) == UNKNOWN_CELL)
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
            t_cell_type cell_value = GET_CELL(board, neighbor_cells[k]);
            if (cell_value == MINE)
                mine_counter++;
            else if (cell_value == UNKNOWN_CELL)
                unknown_counter++;
        }
    t_neighbors_data neighbors_data = {unknown_counter, mine_counter};
    return neighbors_data;
}

t_ptr_matrix initialize_zeros_matrix(t_matrix_size matrix_size) {
    t_ptr_matrix matrix = (t_ptr_matrix)malloc(sizeof(double) * matrix_size.x * matrix_size.y);
    for (int i = 0; i < matrix_size.x; i++)
        for (int j = 0; j < matrix_size.y; j++) {
            t_matrix_cell cell = {i, j};
            SET_CELL(matrix, cell, 0);
        }
    return matrix;
}

t_matrix_size get_matrix_dimensions(t_ptr_board board) {
    t_matrix_size equations_dimensions = {0, 1};
    t_ptr_matrix is_cell_variable_marked = initialize_zeros_matrix(board_size);
    for (int i = 0; i < board_size.x; i++)
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            bool is_cell_equation = false;
            if (!is_numeric_cell(board, cell))
                continue;
            t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
            for (int k = 0; k < NEIGHBORS_NUMBER; k++) {
                if (is_cell_in_board(neighbor_cells[k])
                && GET_CELL(board, neighbor_cells[k]) == UNKNOWN_CELL) {
                    is_cell_equation = true;
                    if (GET_CELL(is_cell_variable_marked, neighbor_cells[k]) == false) {
                        SET_CELL(is_cell_variable_marked, neighbor_cells[k], true);
                        equations_dimensions.y += 1;
                    }
                }
            }
            if (is_cell_equation)
                equations_dimensions.x += 1;
        }
    free(is_cell_variable_marked);
    return equations_dimensions;
}

int get_number_of_deterministic_variables(t_ptr_matrix matrix, t_matrix_size matrix_size) {
    int deterministic_variables = 0;
    for (int i = 0; i < matrix_size.x;  i++) {
        int number_of_non_zero_row_variables = 0;
        for (int j = 0; j < matrix_size.y - 1; j++) {
            t_matrix_cell cell = {i, j};
            if (GET_CELL(matrix, cell) != 0)
                number_of_non_zero_row_variables++;
        }
        if (number_of_non_zero_row_variables == 1)
            return deterministic_variables++;
    }
    return deterministic_variables;
}

t_board_cell extract_board_cell(int variable_number, t_ptr_matrix variables_map) {
    t_board_cell cell = {0, 0};
    for (int i = 0; i < board_size.x; i++) {
        for (int j = 0; j < board_size.y; j++) {
            cell.x = i;
            cell.y = j;
            if (GET_CELL(variables_map, cell) == variable_number)
                goto lblReturn;
        }
    }
    lblReturn:
    return cell;
}

void get_deterministic_moves(t_ptr_matrix matrix, t_ptr_matrix variables_map, t_matrix_size matrix_size, t_moves *moves) {
    int number_of_moves_added = 0;
    int number_of_moves = get_number_of_deterministic_variables(matrix, matrix_size);
    moves->number_of_moves = number_of_moves;
    moves->moves = (t_move*)malloc(sizeof(t_move) * number_of_moves);
    int row = 0;
    while (number_of_moves_added < number_of_moves) {
        int number_of_non_zero_row_variables = 0;
        for (int col = 0; col < matrix_size.y - 1; col++) {
            t_matrix_cell cell = {row, col};
            if (GET_CELL(matrix, cell) != 0)
                number_of_non_zero_row_variables++;
        }
        if (number_of_non_zero_row_variables == 1)
            for (int col = 0; col < matrix_size.y - 1; col++) {
                t_matrix_cell cell = {row, col};
                if (GET_CELL(matrix, cell) != 0) {
                    t_matrix_cell bias_cell = {row, matrix_size.y - 1};
                    t_move move = {extract_board_cell(col, variables_map), (bool)GET_CELL(matrix, bias_cell)};
                    moves->moves[number_of_moves_added] = move;
                    number_of_moves_added++;
                }
            }
        row++;
    }
}

t_ptr_matrix fill_matrix(t_ptr_board board, t_ptr_matrix matrix, t_matrix_size matrix_size) {
    int variables_counter = 1;
    int current_equation = 0;
    t_ptr_matrix variables_map = initialize_zeros_matrix(board_size);
    for (int i = 0; i < board_size.x; i++) {
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            if (!is_numeric_cell(board, cell))
                continue;
            t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
            if (get_neighbors_data(board, neighbor_cells).unknowns == 0)
                continue;
            for (int k = 0; k < NEIGHBORS_NUMBER; k++)
            {
                if (!is_cell_in_board(neighbor_cells[k]))
                    continue;
                if (GET_CELL(board, neighbor_cells[k]) == UNKNOWN_CELL) {
                    if (GET_CELL(variables_map, neighbor_cells[k]) == 0) {
                        SET_CELL(variables_map, neighbor_cells[k], variables_counter);
                        variables_counter++;
                    }
                    t_board_cell matrix_cell = {current_equation, GET_CELL(variables_map, neighbor_cells[k])};
                    SET_CELL(matrix, matrix_cell, 1);
                }
            }
            t_board_cell bias_cell = {current_equation, matrix_size.y - 1};
            SET_CELL(matrix, bias_cell, GET_CELL(board, cell));
            current_equation++;
        }
    }
    return variables_map;
}

void gauss_elimination(t_ptr_matrix matrix, t_matrix_size matrix_size){
    for(int i = 0; i < matrix_size.x - 1; i++) {
        for(int k = i + 1; k < matrix_size.x; k++) {
            t_matrix_cell trace_cell = {i, i};
            t_matrix_cell numerator_cell = {k, i};
            ASSERT(GET_CELL(matrix, trace_cell) != 0)
            double ratio_factor = GET_CELL(matrix, numerator_cell) / GET_CELL(matrix, trace_cell);
            for(int j = 0; j < matrix_size.y; j++) {
                t_matrix_cell fixed_cell = {k, j};
                t_matrix_cell balanced_cell = {i, j};
                SET_CELL(matrix, fixed_cell,
                        GET_CELL(matrix, fixed_cell) - (ratio_factor * (GET_CELL(matrix, balanced_cell))));
            }
        }
    }

}

bool is_legal_board(t_ptr_board board) {
    for (int i = 0; i < board_size.x; i++)
        for (int j = 0; j < board_size.y; j++) {
            t_board_cell cell = {i, j};
            if (is_numeric_cell(board, cell)) {
                t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
                t_neighbors_data neighbors_data = get_neighbors_data(board, neighbor_cells);
                int cell_number = GET_CELL(board, cell);
                if (neighbors_data.mines > cell_number || cell_number > neighbors_data.mines + neighbors_data.unknowns)
                    return false;
            }
        }
    return true;
}

void update_board_by_moves(t_ptr_board board, t_moves moves) {
    for (int i = 0; i < moves.number_of_moves; i++) {
        t_move move = moves.moves[i];
        if (move.is_mine)
            SET_CELL(board, move.cell, MINE);
    }
}

t_error_code get_moves(t_ptr_board board, t_moves *moves, int total_number_of_mines) {
    if (!is_legal_board(board))
        return ERROR_GET_MOVE_ILLEGAL_BOARD_DETECTED;
    t_board_size matrix_dimensions = get_matrix_dimensions(board);
    t_ptr_matrix matrix = initialize_zeros_matrix(matrix_dimensions);
    t_ptr_matrix variables_map = fill_matrix(board, matrix, matrix_dimensions);
    gauss_elimination(matrix, matrix_dimensions);
    if (get_number_of_deterministic_variables(matrix, matrix_dimensions) > 0)
        get_deterministic_moves(matrix, variables_map, matrix_dimensions, moves);
    else {
        return 0;
    }
    t_error_code error_code = log_moves(*moves);
    if (error_code)
        return error_code;
    update_board_by_moves(board, *moves);
    return RETURN_CODE_SUCCESS;
}












