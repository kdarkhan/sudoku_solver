all: sudoku_solver
sudoku_solver: sudoku_solver.c
	gcc -g -o sudoku_solver sudoku_solver.c
clean:
	rm -f sudoku_solver
