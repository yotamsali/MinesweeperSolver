#ifndef MINESWEEPERSOLVER_BOARD_ANALYZER_H
#define MINESWEEPERSOLVER_BOARD_ANALYZER_H

#include <stdbool.h>
#include "minesweeper_solver.h"

#define NULL_MAP_CELL -1
#define FINAL_POSITIVE 1
#define FINAL_NEGATIVE 0
#define NEIGHBORS_NUMBER 8

t_move get_move(t_ptr_board board);

void update_best_unsafe_move(t_board_cell * best_unsafe_move_cell,
                             float * best_unsafe_move_rank,
                             bool * is_best_unsafe_move_mine,
                             t_board_cell cell);

t_ptr_map initialize_map(t_ptr_board board);

void fill_maps(t_ptr_board board, t_ptr_map mine_map, t_ptr_map clear_map);

void set_neighbors_map_values(t_ptr_board board,
                              t_ptr_map mine_map,
                              t_ptr_map clear_map,
                              t_board_cell cell,
                              t_board_cell neighbor_cells[],
                              unsigned short neighbor_mines_counter,
                              unsigned short neighbor_clears_counter);

t_move select_optimal_move(t_ptr_map mine_map, t_ptr_map clear_map);

unsigned short get_neighbor_mines_counter(t_ptr_board board, t_board_cell neighbor_cells[]);

unsigned short get_neighbor_clears_counter(t_ptr_board board, t_board_cell neighbor_cells[]);

unsigned short get_neighbor_unknowns_counter(t_ptr_board board, t_board_cell neighbor_cells[]);

bool is_cell_in_board(t_board_cell cell);

bool is_numeric_cell(t_ptr_board board, t_board_cell cell);

#endif //MINESWEEPERSOLVER_BOARD_ANALYZER_H
