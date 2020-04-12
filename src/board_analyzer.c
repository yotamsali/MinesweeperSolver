#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "common.h"
#include "board.h"
#include "board_analyzer.h"
#include "logger.h"

#define KNOWN_MAP_CELL -1
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
    for (int i = 0; i < board_size._x; i++)
        for (int j = 0; j < board_size._y; j++) {
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
    if (cell._x >= board_size._x || cell._x < 0)
        return false;
    if (cell._y >= board_size._y || cell._y < 0)
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

t_move get_random_unsafe_move(t_ptr_map clear_map, float general_clear_probability, t_move fallback_move) {
    t_move random_move = fallback_move;
    int possible_random_cells_counter = 0;
    int random_cell_search_counter = 0;
    for (int i = 0; i < board_size._x; i++)
        for (int j = 0; j < board_size._y; j++) {
            t_board_cell cell = {i, j};
            if (GET_CELL(clear_map, cell) == general_clear_probability)
                possible_random_cells_counter++;
        }
    srand(time(NULL));
    int random_index = rand() % possible_random_cells_counter;
    for (int i = 0; i < board_size._x; i++)
        for (int j = 0; j < board_size._y; j++) {
            t_board_cell cell = {i, j};
            if (GET_CELL(clear_map, cell) == general_clear_probability) {
                if (random_cell_search_counter == random_index) {
                    random_move._cell = cell;
                    goto lblReturn;
                }
                random_cell_search_counter++;
            }
        }
    lblReturn:
    return random_move;
}

t_move
select_optimal_move(t_ptr_map mine_map, t_ptr_map clear_map, float general_clear_probability, t_ptr_board board) {
    t_board_cell best_unsafe_cell = {0, 0};
    float best_unsafe_probability = 0;
    for (int i = 0; i < board_size._x; i++) {
        for (int j = 0; j < board_size._y; j++) {
            t_board_cell current_cell = {i, j};
            float mine_probability = GET_CELL(mine_map, current_cell);
            float clear_probability = GET_CELL(clear_map, current_cell);
            if (mine_probability == KNOWN_MAP_CELL || clear_probability == KNOWN_MAP_CELL)
                continue;
            if (mine_probability == FINAL_POSITIVE || clear_probability == FINAL_POSITIVE) {
                t_move selected_deterministic_move = {current_cell, (bool) mine_probability};
                return selected_deterministic_move;

            }
            if (clear_probability > best_unsafe_probability) {
                best_unsafe_cell = current_cell;
                best_unsafe_probability = clear_probability;
            }
        }
    }
    t_move best_unsafe_move = {best_unsafe_cell, false};
    if (best_unsafe_probability == general_clear_probability)
        return get_random_unsafe_move(clear_map, general_clear_probability, best_unsafe_move);
    return best_unsafe_move;
}

t_ptr_map initialize_map(t_ptr_board board, float general_clear_probability, bool is_mine_map) {
    t_ptr_map map = malloc(sizeof(float) * board_size._x * board_size._y);
    for (int i = 0; i < board_size._x; i++)
        for (int j = 0; j < board_size._y; j++) {
            t_board_cell cell = {i, j};
            if (GET_CELL(board, cell) != UNKNOWN_CELL)
                SET_CELL(map, cell, KNOWN_MAP_CELL);
            else {
                if (is_mine_map)
                    SET_CELL(map, cell, 1 - general_clear_probability);
                else
                    SET_CELL(map, cell, general_clear_probability);
            }
        }
    return map;
}

void update_map_cell(t_ptr_map map, t_board_cell cell, float probability) {
    float cell_value = GET_CELL(map, cell);
    if (cell_value != KNOWN_MAP_CELL && cell_value != FINAL_POSITIVE && cell_value != FINAL_NEGATIVE)
        if (probability > GET_CELL(map, cell))
            SET_CELL(map, cell, probability);
}

void set_neighbors_map_values(t_ptr_board board, t_ptr_map mine_map, t_ptr_map clear_map, t_board_cell cell,
                              t_board_cell neighbor_cells[], t_neighbors_data neighbors_data) {
    t_cell_type cell_number = GET_CELL(board, cell);
    ASSERT(cell_number >= neighbors_data.mines && cell_number <= neighbors_data.mines + neighbors_data.unknowns);
    bool all_neighbor_unknowns_clears = (cell_number == neighbors_data.mines);
    bool all_neighbor_unknowns_mines = (cell_number == neighbors_data.mines + neighbors_data.unknowns);
    if (neighbors_data.unknowns == 0)
        return;
    if (all_neighbor_unknowns_clears || all_neighbor_unknowns_mines) {
        for (int k = 0; k < NEIGHBORS_NUMBER; k++)
            if (is_cell_in_board(neighbor_cells[k]))
                if (GET_CELL(board, neighbor_cells[k]) == UNKNOWN_CELL)
                    if (all_neighbor_unknowns_clears) {
                        SET_CELL(mine_map, neighbor_cells[k], FINAL_NEGATIVE);
                        SET_CELL(clear_map, neighbor_cells[k], FINAL_POSITIVE);
                    } else {
                        SET_CELL(mine_map, neighbor_cells[k], FINAL_POSITIVE);
                        SET_CELL(clear_map, neighbor_cells[k], FINAL_NEGATIVE);
                    }
        return;
    }
    float mine_probability = (float) (cell_number - neighbors_data.mines) / (float) neighbors_data.unknowns;
    for (int k = 0; k < NEIGHBORS_NUMBER; k++)
        if (is_cell_in_board(neighbor_cells[k])) {
            update_map_cell(mine_map, neighbor_cells[k], mine_probability);
            update_map_cell(clear_map, neighbor_cells[k], 1 - mine_probability);
        }
}

void fill_maps(t_ptr_board board, t_ptr_map mine_map, t_ptr_map clear_map) {
    t_board_cell cell;
    for (int i = 0; i < board_size._x; i++) {
        for (int j = 0; j < board_size._y; j++) {
            cell._x = i;
            cell._y = j;
            if (!is_numeric_cell(board, cell))
                continue;
            t_board_cell neighbor_cells[] = NEIGHBOR_CELLS(cell);
            t_neighbors_data neighbors_data = get_neighbors_data(board, neighbor_cells);
            set_neighbors_map_values(board, mine_map, clear_map,
                                     cell, neighbor_cells, neighbors_data);
        }
    }
}

bool is_legal_board(t_ptr_board board) {
    for (int i = 0; i < board_size._x; i++)
        for (int j = 0; j < board_size._y; j++) {
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

void update_board_by_move(t_ptr_board board, t_move move) {
    if (move._is_mine)
        SET_CELL(board, move._cell, MINE);
}

t_error_code get_move(t_ptr_board board, t_move *move, int total_number_of_mines) {
    if (!is_legal_board(board))
        return ERROR_GET_MOVE_ILLEGAL_BOARD_DETECTED;
    float general_clear_probability = get_general_clear_probability(total_number_of_mines, board);
    t_ptr_map mine_map = initialize_map(board, general_clear_probability, true);
    t_ptr_map clear_map = initialize_map(board, general_clear_probability, false);
    fill_maps(board, mine_map, clear_map);
    t_error_code error_code = log_probability_maps(mine_map, clear_map);
    if (error_code)
        return error_code;
    *move = select_optimal_move(mine_map, clear_map, general_clear_probability, board);
    free(mine_map);
    free(clear_map);
    error_code = log_move(*move);
    if (error_code)
        return error_code;
    update_board_by_move(board, *move);
    return RETURN_CODE_SUCCESS;
}













