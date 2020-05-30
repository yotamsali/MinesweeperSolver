/**************************************************************************************************
 * @file matrix.h
 * @project MinesweeperSolver
 * @author Yotam Sali
 * @date 25.5.2020
 * @brief Header of matrix module, exports basic double type matrix utilities.
**************************************************************************************************/
#ifndef MINESWEEPERSOLVER_MATRIX_H
#define MINESWEEPERSOLVER_MATRIX_H

#include "board.h"

/**
 * Macro for a cell (x_index, y_index) of matrix.
 */
#define MATRIX_CELL(matrix, x_index, y_index) matrix.data[x_index * matrix.size.cols + y_index]

typedef t_board_size t_matrix_size;
typedef t_board_cell t_matrix_cell;
typedef double *t_data;
typedef struct {
    t_data data;
    t_matrix_size size;
} t_matrix;

/**
 * @brief Initialize double matrix pointer in heap.
 * @param matrix_size Requested matrix size struct.
 * @param fill - double value of all matrix cells (initialized default).
 * @return Double matrix pointer.
 */
t_matrix initialize_matrix(t_matrix_size matrix_size, double fill);

/**
 * @brief Perform Gaussian elimination algorithm over a matrix.
 * For further details read https://en.wikipedia.org/wiki/Gaussian_elimination.
 * @param matrix Matrix to perform Gaussian elimination on.
 * @return Void.
 */
void gauss_eliminate(t_matrix matrix);

/**
 * @brief Get last row index of a matrix which is not a zeros row.
 * @param matrix - Searched matrix.
 * @return Last row index (Integer) which is not a zeros row in the matrix.
 */
int get_last_non_zero_row(t_matrix matrix);

/**
 * @brief Get upper bound for row equation with 1-0 variable range.
 * @param matrix - The matrix.
 * @param row - Row index.
 * @return Upper bound for row equation with 1-0 variable range.
 */
double get_row_upper_bound(t_matrix matrix, int row);

/**
 * @brief Get lower bound for row equation with 1-0 variable range.
 * @param matrix - The matrix.
 * @param row - Row index.
 * @return Lower bound for row equation with 1-0 variable range.
 */
double get_row_lower_bound(t_matrix matrix, int row);

/**
 * @brief Get matrix row sum.
 * @param matrix - The matrix.
 * @param row - Row index.
 * @return Row sum.
 */
double get_row_sum(t_matrix matrix, int row);

#endif //MINESWEEPERSOLVER_MATRIX_H
