/*
* author: Darkhan Kubigenov
*
* The code solves sudoku puzzles
* given to stdin
* First the user needs to enter number of puzzles
*	and then all the puzzles. 
* An empty cell is represented by any number less than 1
* 	or greater than 9
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//constant - number with 9 ones in binary representation
int NINE_ONES = (1<<9) - 1;

//mutex to lock stdout
pthread_mutex_t mutex_stdout;

/*
* Allocates memory to be used to store a puzzle
*/
int ** get_puzzle_mem()
{
    int ** result = malloc(sizeof(int * ) * 9);
    int i;
    for(i=0; i<9; i++)
    {
		result[i] = malloc(sizeof(int ) * 9);
    }
    return result;
}

/*
* Frees the memory allocated by get_puzzle_mem()
*/
void free_puzzle_mem(int ** puzzle)
{
    int i;
    for(i=0; i<9; i++)
    {
		free(puzzle[i]);
    }
    free(puzzle);
    return;
}

/*
* Given a number the function returns 
* a number where number-th bit from the left in binary representation
* is equal to 1 and the rest of bits are zero. If the number
* is not in the range between 1 and 9 the function 
* returns a number with 9 one's
*/
int make_singleton(int number)
{
    if(number > 0 && number <= 9)
		return (1<<(number - 1));
    else return NINE_ONES;
}

/*
* Function returns 1 if binary representation of the number
* has only one 1
* and returns 0 otherwise
*/
int is_singleton(int number)
{
    int check = 1;
    int found = 0;
    int i;
    for(i=0; i<9; i++)
    {
		if((check & number) != 0)
	    	found++;
		check <<= 1;
    }
    // This happens if the number is invalid and has no ones in 9 least significant bits
    if(found < 1)
    {
		fprintf(stderr, "Wrong input in is_singleton: %d\n", number);
		return 0;
    }
    return (found == 1) ? 1:0; 
}

/*
* Given an index the function returns the left/upper index of 3x3 box
* inside which a cell with the given index lies
*/
int get_box_start_index(int index)
{
    return (index/3) * 3; //using integer division to make it work as intended
}


/*
* Checks if the given puzzle is a legal one
* returns 0 if it is illegal
* returns 1 otherwise
*/
int is_legal_puzzle(int ** puzzle)
{
    int i, j;
    for(i=0; i<9; i++)
    {
    	for(j=0; j<9; j++)
		{
			int value = puzzle[i][j];
			if( value>0 && value<=9) //if the cell is not empty
				{
					int k, l;
					for(k=0; k<9; k++)
					{
						// check the row
						if(k!=j)
							if(value == puzzle[i][k])
								return 0;
						// check the column
						if(k!=i)
							if(value == puzzle[k][j])
								return 0;
					}
					// check 3x3 box
					int ii = get_box_start_index(i);
					int jj = get_box_start_index(j);
					for(k=ii; k<ii+3; k++)
						for(l=jj; l<jj+3; l++)
							if(k!=i || l!=j)
								if(value == puzzle[k][l])
									return 0;
				}
		}
    }

	return 1; //no errors, so the puzzle is legal
}
/*
* Given the number used in sudoku solver method_one
* the function returns number represented by it
*/
int from_singleton(int number)
{
    int check = 1, i;
    for(i=1; i<10; i++, check <<= 1)
    {
		if((check & number)!= 0)
	    	return i;
    }
    printf("Wrong number:\n %d is not a valid number\n", number);
    return 0;
}



/*
* First method which works as follows:
* If it is certain that one cell can contain only one possible number
*	remove the possibility of having that number
*	in the row, column, and 3x3 box
*	where that cell resides
*/
int method_one(int ** puzzle)
{
    int changed = 0;
    int i;
    for(i=0; i<9; i++)
    {
		int j;
		for(j=0; j<9; j++)
		{
	    	int value = puzzle[i][j];
	    	if(is_singleton(value)>0)
	    	{
				int k;
				for(k=0; k<9; k++)
				{
		    		if(i!=k)
		    		{
						if(value & puzzle[k][j])
						{
			    			changed = 1;
			    			int temp = puzzle[k][j];
			    			puzzle[k][j] &= (~value);
						}
		    		}
		    		if(j!=k)
		    		{
						if(value & puzzle[i][k])
						{
			    			changed = 1;
			    			int temp = puzzle[i][k];
			    			puzzle[i][k] &= (~value);
						}
		    		}
				}

				int ii = get_box_start_index(i);
				int jj = get_box_start_index(j);
				for(k = ii; k < ii+3; k++)
				{
		    		int l;
		    		for(l=jj; l<jj+3; l++)
		    		{
						if(k!=i || l!=j)
						{
			    			if(value & puzzle[k][l])
			    			{
								changed = 1;
								puzzle[k][l] &= (~value);
			    			}
						}
		    		}
				}
	    	}
		}
    }
    return changed;
}

int method_two(int ** puzzle)
{
	int changed=0;
	int i, j;
	for(i=0; i<9; i++)
	{
		for(j=0; j<9; j++)
		{
			int value = puzzle[i][j];
			if( !is_singleton(value))
			{
				int row_pos_values = 0, col_pos_values, k;

				// omit from the row and from the column 
				for(k=0; k<9; k++)
				{
					if(k!=j)
						row_pos_values |= puzzle[i][k];
					if(k!=i)
						col_pos_values |= puzzle[k][j];
				}
				if(row_pos_values != NINE_ONES)
				{
					if(puzzle[i][j] & (~row_pos_values) != puzzle[i][j])
					{
						puzzle[i][j] &= (~row_pos_values);
						changed = 1;
					}
				}
				if(col_pos_values != NINE_ONES)
				{
					if(puzzle[i][j] & (~col_pos_values) != puzzle[i][j])
					{
						puzzle[i][j] &= (~col_pos_values);
						changed = 1;
					}
				}

				// omit from the box
				int ii = get_box_start_index(i);
				int jj = get_box_start_index(j);
				int m,n;
				int box_pos_values=0;
				for(m=ii; m<ii+3; m++)
					for(n=jj; n<jj+3; n++)
						if(m!=i || n!=j)
							box_pos_values |= puzzle[m][n];
				if(box_pos_values != NINE_ONES)
				{
					if(puzzle[i][j] & (~box_pos_values) != puzzle[i][j])
					{
						puzzle[i][j] &= (~box_pos_values);
						changed = 1;
					}
				}


			}
		}
	}
	return changed;
}

/*
* given a puzzle and pointer to malloc'd space
* stores in that malloced space transformed
* puzzle which is used by solve_sudoku()
*/
void transform(int ** original, int ** result)
{
    int i;
    for(i=0; i<9; i++)
    {
		int j;
		for(j=0; j<9; j++)
		{
	    	result[i][j] = make_singleton(original[i][j]);
		}
    }
}


void print_puzzle_transformed(int ** puzzle)
{
    int i;
    for(i=0; i<9; i++)
    {
		int j;
		for(j=0; j<9; j++)
		{
	    	printf("%d\t", from_singleton(puzzle[i][j]));
		}
		printf("\n");
    }
}

void print_puzzle(int ** puzzle)
{
    int i;
    for(i=0; i<9; i++)
    {
	int j;
	for(j=0; j<9; j++)
	{
	    printf("%d\t", puzzle[i][j]);
	}
	printf("\n");
    }
}

void solve_sudoku(int ** puzzle)
{
	// check if the puzzle is legal
	if(!is_legal_puzzle(puzzle))
	{
		fprintf(stderr, "Illegal puzzle input\n");
		return;
	}

    int ** puzzle_transformed = get_puzzle_mem();
    transform(puzzle, puzzle_transformed);

    while(1)
    {
		int changed = method_one(puzzle_transformed);
		changed &= method_one(puzzle_transformed);
		if(!changed)
			break;
    }
	
	print_puzzle_transformed(puzzle_transformed);

}

int main(int argc, char ** argv)
{


    int puzzles_count;
    scanf("%d", &puzzles_count);

	pthread_mutex_init(&mutex_stdout, NULL); // initialize the mutex

	// check the correctness of puzzles_count
	if(puzzles_count > 50)
	{
		fprintf(stderr, "Too many puzzles to solve.\n");
		return 1;
	}
	if(puzzles_count < 1)
	{
		fprintf(stderr, "Too few puzzles to solve.\n");
		return 1;
	}

	// allocate memory to store pointers to puzzles
    int *** puzzles = malloc(sizeof(int **) * puzzles_count);
    int i;
    for(i=0; i<puzzles_count; i++)
    {
		puzzles[i] = get_puzzle_mem();
		int j;
		for(j=0; j<9; j++)
		{
	    	int k;
	    	for(k=0; k<9; k++)
	    	{
				scanf("%u", &puzzles[i][j][k]);
	    	}
		}
    }

	printf("Solved puzzles:\n");
    for(i=0; i<puzzles_count; i++)
    {
		solve_sudoku(puzzles[i]);
		free_puzzle_mem(puzzles[i]);
    }


	pthread_mutex_destroy(&mutex_stdout);
	free(puzzles);
    return 0;
}
