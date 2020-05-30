# MinesweeperSolver

Automated end-to-end Minesweeper solver in Windows enviroment (written in C).
The solver implements the image-processing of board detection, move execution and planning.

## Building
In order to build and execute MinesweeperSolver do the following:

### Prerequisites
You must have Windows enviroment and gdi32.dll, CMake, and Make installed over your machince.

### Build
```bash
git clone https://github.com/yotamsali/MinesweeperSolver.git
cmake MinesweeperSolver
make
```

### Execution
```bash
MinesweeperSolver.exe {level}
```
The parameter level can be either 'beginner', 'intermediate' or 'expert'.
If Minesweeper app is not opened once executing, verify that the 'Minesweeper X.exe' relative path to MinesweeperSolver, is as stated in src/hard_coded_config.h.
Otherwise, change the "MINESWEEPER_PATH" parameter.
The 'Minesweeper X.exe' exact version is critical for this solver, and it is cloned inside 'minesweeper' directory.


## Design

MinesweeperSolver design is based on the following modules:

### Board
Responsible for detection of board cells and game status (smiley state).
The module's way of action is taking a screenshot of Minesweeper window, spliting into cells,
and detecting each cell based on special "magic" colors that specify every cell type.
Detecting smiley is based on yellow-back colors ratio around the smiley.


## Error Handling

In case of unexpected result the program will return a non-zero error code.
All error codes are stated in '''src/error_codes.h''' file.

## Logging 

Olm doesn't generate random numbers itself. Instead the caller must
provide the random data. This makes it easier to port the library to different
platforms since the caller can use whatever cryptographic random number
generator their platform provides.


## Bug reports

Please file bug reports at https://github.com/matrix-org/olm/issues

## Authors

The software may be subject to the U.S. export control laws and regulations
and by downloading the software the user certifies that he/she/it is
authorized to do so in accordance with those export control laws and
regulations.
