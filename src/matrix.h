#ifndef MINESWEEPERSOLVER_MATRIX_H
#define MINESWEEPERSOLVER_MATRIX_H

#include "board.h"

#define MATRIX_CELL(matrix, x_index, y_index) matrix.data[x_index * matrix.size.y + y_index]

typedef t_board_size t_matrix_size;
typedef t_board_cell t_matrix_cell;
typedef double *t_data;
typedef struct {
    t_data data;
    t_matrix_size size;
} t_matrix;

t_matrix initialize_matrix(t_matrix_size matrix_size, double fill);

void gauss_eliminate(t_matrix matrix);

int get_last_non_zero_row(t_matrix matrix);

double get_row_upper_bound(t_matrix matrix, int row);

double get_row_lower_bound(t_matrix matrix, int row);

#endif //MINESWEEPERSOLVER_MATRIX_H
