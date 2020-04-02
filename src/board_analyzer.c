#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "board.h"
#include "board_analyzer.h"

#define NULL_MAP_CELL -1
#define FINAL_POSITIVE 1
#define FINAL_NEGATIVE 0
#define NEIGHBORS_NUMBER 8

#define NEIGHBOR_CELLS(cell) {{cell._x - 1, cell._y - 1},   \
                                {cell._x - 1, cell._y},     \
                                {cell._x - 1, cell._y + 1}, \
                                {cell._x, cell._y - 1},     \
                                {cell._x, cell._y + 1},     \
                                {cell._x + 1, cell._y - 1}, \
                                {cell._x + 1, cell._y},     \
                                {cell._x + 1, cell._y + 1}} \

typedef float *t_ptr_map;

bool is_numeric_cell(t_ptr_board board, t_board_cell cell) {
    unsigned short cell_value = GET_CELL(board, cell);
    return cell_value != MINE && cell_value != UNKNOWN_CELL;
}

bool is_cell_in_board(t_board_cell cell) {
    if (cell._x > board_size._x || cell._x < 0)
        return false;
    if (cell._y > board_size._y || cell._y < 0)
        return false;
    return true;
}

unsigned short get_neighbor_clears_counter(t_ptr_board board, t_board_cell neighbor_cells[]) {
    unsigned short clear_counter = 0;
    for (int k = 0; k < NEIGHBORS_NUMBER; k++)
        if (is_cell_in_board(neighbor_cells[k])) {
            unsigned short cell_value = GET_CELL(board, neighbor_cells[k]);
            if (cell_value != MINE && cell_value != UNKNOWN_CELL)
                clear_counter++;
        }
    return clear_counter;
}

unsigned short get_neighbor_unknowns_counter(t_ptr_board board, t_board_cell neighbor_cells[]) {
    unsigned short unknown_counter = 0;
    for (int k = 0; k < NEIGHBORS_NUMBER; k++)
        if (is_cell_in_board(neighbor_cells[k])) {
            unsigned short cell_value = GET_CELL(board, neighbor_cells[k]);
            if (cell_value == UNKNOWN_CELL)
                unknown_counter++;
        }
    return unknown_counter;
}

t_move select_optimal_move(t_ptr_map mine_map, t_ptr_map clear_map) {
    t_move selected_move;
    t_board_cell current_cell;
    t_board_cell best_unsafe_cell = {0, 0};
    float best_unsafe_probability = 0;
    bool is_best_unsafe_cell_mine = false;
    for (unsigned short i = 0; i < board_size._x; i++) {
        for (unsigned short j = 0; j < board_size._y; j++) {
            current_cell._x = i;
            current_cell._y = j;
            float mine_probability = GET_CELL(mine_map, current_cell);
            float clear_probability = GET_CELL(clear_map, current_cell);
            if (mine_probability == NULL_MAP_CELL || clear_probability == NULL_MAP_CELL)
                continue;
            if (mine_probability == FINAL_POSITIVE || clear_probability == FINAL_POSITIVE) {
                selected_move._is_mine = (bool) mine_probability;
                selected_move._cell = current_cell;
                return selected_move;
            }
            if (mine_probability > best_unsafe_probability || clear_probability > best_unsafe_probability) {
                is_best_unsafe_cell_mine = (clear_probability >= mine_probability);
                best_unsafe_cell = current_cell;
                best_unsafe_probability = fmax(mine_probability, clear_probability);
            }
        }
    }
    selected_move._is_mine = is_best_unsafe_cell_mine;
    selected_move._cell = best_unsafe_cell;
    return selected_move;
}

unsigned short get_neighbor_mines_counter(t_ptr_board board, t_board_cell neighbor_cells[]) {
    unsigned short mine_counter = 0;
    for (int k = 0; k < NEIGHBORS_NUMBER; k++)
        if (is_cell_in_board(neighbor_cells[k]))
            if (GET_CELL(board, neighbor_cells[k]) == MINE)
                mine_counter++;
    return mine_counter;
}

t_ptr_map initialize_map(t_ptr_board board) {
    t_ptr_map map = malloc(sizeof(float) * board_size._x * board_size._y);
    for (int i = 0; i < board_size._x; i++)
        for (int j = 0; j < board_size._y; j++)
            map[i * board_size._x + j] = NULL_MAP_CELL;
    return map;
}

void set_neighbors_map_values(t_ptr_board board,
                              t_ptr_map mine_map,
                              t_ptr_map clear_map,
                              t_board_cell cell,
                              t_board_cell neighbor_cells[],
                              unsigned short neighbor_mines_counter,
                              unsigned short neighbor_clears_counter) {
    unsigned short cell_number = GET_CELL(board, cell);
    ASSERT(cell_number >= neighbor_mines_counter);
    if (cell_number == neighbor_mines_counter) {
        for (int k = 0; k < NEIGHBORS_NUMBER; k++)
            if (is_cell_in_board(neighbor_cells[k]))
                if (GET_CELL(board, neighbor_cells[k]) == UNKNOWN_CELL) {
                    SET_CELL(mine_map, neighbor_cells[k], FINAL_NEGATIVE)
                    SET_CELL(clear_map, neighbor_cells[k], FINAL_POSITIVE)
                }
        return;
    }
    unsigned short neighbor_unknowns_counter = get_neighbor_unknowns_counter(board, neighbor_cells);
    float mine_probability = (float) (cell_number - neighbor_mines_counter) / neighbor_unknowns_counter;
    for (int k = 0; k < NEIGHBORS_NUMBER; k++)
        if (is_cell_in_board(neighbor_cells[k]))
            if (GET_CELL(board, neighbor_cells[k]) == UNKNOWN_CELL) {
                SET_CELL(mine_map, neighbor_cells[k], mine_probability)
                SET_CELL(clear_map, neighbor_cells[k], 1 - mine_probability)
            }
}

void fill_maps(t_ptr_board board, t_ptr_map mine_map, t_ptr_map clear_map) {
    t_board_cell cell;
    t_board_cell neighbor_cell;
    unsigned short neighbor_mines_counter = 0;
    unsigned short neighbor_clears_counter = 0;
    for (unsigned short i = 0; i < board_size._x; i++) {
        for (unsigned short j = 0; j < board_size._y; j++) {
            cell._x = i;
            cell._y = j;
            if (!is_numeric_cell(board, cell))
                continue;
            t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
            neighbor_mines_counter = get_neighbor_mines_counter(board, neighbor_cells);
            neighbor_clears_counter = get_neighbor_clears_counter(board, neighbor_cells);
            set_neighbors_map_values(board, mine_map, clear_map,
                                     cell, neighbor_cells, neighbor_mines_counter, neighbor_clears_counter);
        }
    }
}

bool is_legal_board(t_ptr_board board) {
    for (unsigned short i = 0; i < board_size._x; i++)
        for (unsigned short j = 0; j < board_size._y; j++) {
            t_board_cell cell = {i, j};
            if (is_numeric_cell(board, cell)){
                t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
                unsigned short mines_counter = get_neighbor_mines_counter(board, neighbor_cells);
                if (mines_counter > GET_CELL(board, cell))
                    return false;
            }
        }
    return true;
}

t_error_code get_move(t_ptr_board board, t_move * move) {
    if (!is_legal_board(board))
        return ERROR_GET_MOVE_ILLEGAL_BOARD_DETECTED;
    t_ptr_map mine_map = initialize_map(board);
    t_ptr_map clear_map = initialize_map(board);
    fill_maps(board, mine_map, clear_map);
    *move = select_optimal_move(mine_map, clear_map);
    free(mine_map);
    free(clear_map);
    return RETURN_CODE_SUCCESS;
}













