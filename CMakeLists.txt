cmake_minimum_required(VERSION 3.1)
project(MinesweeperSolver C)

set(CMAKE_C_STANDARD 99)
set(SOURCES src/minesweeper_solver.c src/minesweeper_solver_utils.c src/commander.c src/board.c src/board_analyzer.c src/board_analyzer.h)
set(HEADERS src/minesweeper_solver_utils.h src/minesweeper_solver.h src/commander.h src/board.h)

add_executable(MinesweeperSolver ${SOURCES} ${HEADERS})
target_link_libraries(MinesweeperSolver gdi32.dll)











