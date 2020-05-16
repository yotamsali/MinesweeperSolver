#ifndef MINESWEEPERSOLVER_MATRIX_H
#define MINESWEEPERSOLVER_MATRIX_H

#include "board.h"

#define MATRIX_CELL(matrix, matrix_size, cell) matrix[cell.x * matrix_size.y + cell.y]

typedef t_board_size t_matrix_size;
typedef t_board_cell t_matrix_cell;
typedef double *t_ptr_matrix;

t_ptr_matrix initialize_matrix(t_matrix_size matrix_size, double fill);

void gauss_eliminate(t_ptr_matrix matrix, t_matrix_size matrix_size);

int get_first_non_zero_row(t_ptr_matrix matrix, t_matrix_size matrix_size);

#endif //MINESWEEPERSOLVER_MATRIX_H
