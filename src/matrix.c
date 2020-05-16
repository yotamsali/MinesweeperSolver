#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "logger.h"

void swap_rows(t_matrix matrix, int row1, int row2) {
    for (int col = 0; col < matrix.size.y; col++) {
        double temp = MATRIX_CELL(matrix, row1, col);
        MATRIX_CELL(matrix, row1, col) = MATRIX_CELL(matrix, row2, col);
        MATRIX_CELL(matrix, row2, col) = temp;
    }
}

void multiply_row(t_matrix matrix, int row, double factor) {
    for (int col = 0; col < matrix.size.y; col++) {
        MATRIX_CELL(matrix, row, col) *= factor;
    }
}

void add_row(t_matrix matrix, int base_row, int added_row) {
    for (int col = 0; col < matrix.size.y; col++) {
        MATRIX_CELL(matrix, base_row, col) += MATRIX_CELL(matrix, added_row, col);
    }
}

void gauss_eliminate(t_matrix matrix) {
    int row = 0;
    int col = 0;
    log_matrix(matrix, "Matrix before gaussian elimination");
    while ((row < matrix.size.x) && (col < matrix.size.y)) {
        int max_row = row;
        for (int current_row = row + 1; current_row < matrix.size.x; ++current_row) {
            if (fabs(MATRIX_CELL(matrix, current_row, col)) > fabs(MATRIX_CELL(matrix, max_row, col))) {
                max_row = current_row;
            }
        }
        if (MATRIX_CELL(matrix, max_row, col) != 0) {
            if (row != max_row)
                swap_rows(matrix, max_row, row);
            double current_value = MATRIX_CELL(matrix, row, col);
            multiply_row(matrix, row, 1 / current_value);
            for (int iter_row = row + 1; iter_row < matrix.size.x; ++iter_row) {
                double multiply_value = -MATRIX_CELL(matrix, iter_row, col);
                if (multiply_value != 0) {
                    multiply_row(matrix, row, multiply_value);
                    add_row(matrix, iter_row, row);
                    multiply_row(matrix, row, 1 / multiply_value);
                }
            }
            row++;
        }
        col++;
    }
    log_matrix(matrix, "Matrix before gaussian elimination");
}

int get_last_non_zero_row(t_matrix matrix) {
    for (int row = matrix.size.x - 1; row >= 0 && row < matrix.size.x; row++) {
        bool is_non_zeros_row = false;
        for (int col = 0; col < matrix.size.y; col++) {
            if (MATRIX_CELL(matrix, row, col) != 0.0) {
                is_non_zeros_row = true;
            }
        }
        if (!is_non_zeros_row)
            return row;
    }
}

t_matrix initialize_matrix(t_matrix_size matrix_size, double fill) {
    t_matrix matrix = {NULL, matrix_size};
    matrix.data = (t_data) malloc(sizeof(double) * matrix_size.x * matrix_size.y);
    if (matrix.data == NULL)
        goto lblReturn;
    for (int i = 0; i < matrix_size.x; i++)
        for (int j = 0; j < matrix_size.y; j++) {
            MATRIX_CELL(matrix, i, j) = fill;
        }
    lblReturn:
    return matrix;
}