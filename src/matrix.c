#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "logger.h"

void swap_rows(t_ptr_matrix matrix, t_matrix_size matrix_size, int row1, int row2) {
    for (int col = 0; col < matrix_size.y; col++) {
        t_matrix_cell celL_row1 = {row1, col}, cell_row2 = {row2, col};
        double temp = MATRIX_CELL(matrix, matrix_size, celL_row1);
        MATRIX_CELL(matrix, matrix_size, celL_row1) = MATRIX_CELL(matrix, matrix_size, cell_row2);
        MATRIX_CELL(matrix, matrix_size, cell_row2) = temp;
    }
}

void multiply_row(t_ptr_matrix matrix, t_matrix_size matrix_size, int row, double factor) {
    for (int col = 0; col < matrix_size.y; col++) {
        t_matrix_cell cell = {row, col};
        MATRIX_CELL(matrix, matrix_size, cell) *= factor;
    }
}

void add_row(t_ptr_matrix matrix, t_matrix_size matrix_size, int base_row, int added_row) {
    for (int col = 0; col < matrix_size.y; col++) {
        t_matrix_cell base_cell = {base_row, col}, added_cell = {added_row, col};
        MATRIX_CELL(matrix, matrix_size, base_cell) += MATRIX_CELL(matrix, matrix_size, added_cell);
    }
}

void gauss_eliminate(t_ptr_matrix matrix, t_matrix_size matrix_size)
{
    int row = 0;
    int col = 0;
    log_matrix(matrix, matrix_size, "Matrix before gaussian elimination");
    while ((row < matrix_size.x) && (col < matrix_size.y)) {
        int max_row = row;
        for (int current_row = row + 1; current_row < matrix_size.x; ++current_row) {
            t_matrix_cell candidate = {current_row, col}, current_max = {max_row, col};
            if (fabs(MATRIX_CELL(matrix, matrix_size, candidate)) > fabs(MATRIX_CELL(matrix, matrix_size, current_max))) {
                max_row = current_row;
            }
        }
        t_matrix_cell pivot = {max_row, col}, current_cell = {row, col};
        if (MATRIX_CELL(matrix, matrix_size, pivot) != 0)
        {
            if(row != max_row)
                swap_rows(matrix, matrix_size, max_row, row);
            double current_value = MATRIX_CELL(matrix, matrix_size, current_cell);
            multiply_row(matrix, matrix_size, row, 1 / current_value);
            for(int iter_row = row + 1; iter_row < matrix_size.x; ++iter_row) {
                t_matrix_cell iter_cell = {iter_row, col};
                double multiply_value = -MATRIX_CELL(matrix, matrix_size, iter_cell);
                if(multiply_value != 0) {
                    multiply_row(matrix, matrix_size, row, multiply_value);
                    add_row(matrix, matrix_size, iter_row, row);
                    multiply_row(matrix, matrix_size, row, 1 / multiply_value);
                }
            }
            row++;
        }
        col++;
    }
    log_matrix(matrix, matrix_size, "Matrix before gaussian elimination");
}

int get_first_non_zero_row(t_ptr_matrix matrix, t_matrix_size matrix_size) {
    for (int row = matrix_size.x - 1; row >= 0 && row < matrix_size.x; row++) {
        bool is_non_zeros_row = false;
        for (int col = 0; col < matrix_size.y; col++) {
            t_matrix_cell cell = {row, col};
            if (MATRIX_CELL(matrix, matrix_size, cell) != 0.0) {
                is_non_zeros_row = true;
            }
        }
        if (!is_non_zeros_row)
            return row;
    }
}

t_ptr_matrix initialize_matrix(t_matrix_size matrix_size, double fill) {
    t_ptr_matrix matrix = (t_ptr_matrix) malloc(sizeof(double) * matrix_size.x * matrix_size.y);
    if (matrix == NULL)
        goto lblReturn;
    for (int i = 0; i < matrix_size.x; i++)
        for (int j = 0; j < matrix_size.y; j++) {
            t_matrix_cell cell = {i, j};
            MATRIX_CELL(matrix, matrix_size, cell) = fill;
        }
    lblReturn:
    return matrix;
}