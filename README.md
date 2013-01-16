
SUDOKU SOLVER

The program reads from stdin the number of puzzles it neads to solve. Then it reads that number of puzzles. Any number not in the range between 1 and 9 is interpreted as empty cell in the puzzle. The program then computes the puzzle and prints the result to the stdout. If multiple solutions are available it prints one of possible solutions.

Any cell from sudoku puzzle represented by one integer where its binary representation corresponds to all possible values in that cell. For instance, if user inputs 5 in a cell, the cell will be represented by a number whose binary representation has 5-th digit from the left 1, and the rest of the digits are ZEROS. An empty cell is represented by a number with 9 rightmost digits equal to 1.
