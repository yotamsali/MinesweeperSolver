cmake_minimum_required(VERSION 3.1)
project(MinesweeperSolver C)

option (DEBUG "Use Debug assertions." ON)

set(CMAKE_C_STANDARD 99)
set(SOURCES src/minesweeper_solver.c src/minesweeper_solver_utils.c src/commander.c src/board.c src/board_analyzer.c src/board_analyzer.h)
set(HEADERS src/minesweeper_solver_utils.h src/commander.h src/board.h src/hard_coded_config.h src/error_codes.h src/common.h)
if(DEBUG)
    add_definitions(-DDEBUG)
endif()

add_executable(MinesweeperSolver ${SOURCES} ${HEADERS})
target_link_libraries(MinesweeperSolver gdi32.dll)











