
#include <stdio.h>
#include <stdlib.h>

unsigned int ** get_puzzle_mem()
{
    unsigned int ** result = malloc(sizeof(unsigned int * ) * 9);
    int i;
    for(i=0; i<9; i++)
    {
	result[i] = malloc(sizeof(unsigned int ) * 9);
    }
    return result;
}

void free_puzzle_mem(unsigned int ** puzzle)
{
    int i;
    for(i=0; i<9; i++)
    {
	free(puzzle[i]);
    }
    free(puzzle);
    return;
}

unsigned int make_singleton(unsigned int number)
{
    if(number > 0 && number <= 9)
	return (1<<(number - 1));
    else return (1<<9) - 1;
}

unsigned int is_singleton(unsigned int number)
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
    // TODO: debugging, delete later
    if(found < 1)
    {
	printf("Something is wrong with the number in is_singleton: %d\n", number);
	return 0;
    }
    return (found == 1) ? 1:0; 
}

unsigned int from_singleton(unsigned int number)
{
    unsigned int check = 1, i;
    for(i=1; i<10; i++, check <<= 1)
    {
	if((check & number)!= 0)
	    return i;
    }
    printf("Wrong number:\n %d is not a singleton\n", number);
    return 0;
}

int get_box_start_index(int index)
{
    return (index/3) * 3;
}

int method_one(unsigned int ** puzzle)
{
    int changed = 0;
    int i;
    for(i=0; i<9; i++)
    {
	int j;
	for(j=0; j<9; j++)
	{
	    unsigned int value = puzzle[i][j];
	    if(is_singleton(value)>0)
	    {
		int k;
		for(k=0; k<9; k++)
		{
		    if(i!=k)
		    {
			if(value & puzzle[k][j] != 0)
			{
			    changed = 1;
			    puzzle[k][j] &= (~value);
			}
		    }
		    if(j!=k)
		    {
			if(value & puzzle[i][k] != 0)
			{
			    changed = 1;
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
			    if(value & puzzle[k][l] != 0)
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

void transform(unsigned int ** original, unsigned int ** result)
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

void print_puzzle(unsigned int ** puzzle)
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

void print_puzzle_original(unsigned int ** puzzle)
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

void solve_sudoku(unsigned int ** puzzle)
{
    unsigned int ** puzzle_tr = get_puzzle_mem();
    transform(puzzle, puzzle_tr);


    printf("Here is the transformed one\n");
    print_puzzle_original(puzzle_tr);

    int counter = 0;
    /*
    while(method_one(puzzle_tr) > 0)
    {
	counter ++;
    }
    */
    method_one(puzzle_tr);

//DEBUG:
    printf("Executed %d times\n", counter);
    print_puzzle_original(puzzle_tr);
    
}

int main(int argc, char ** argv)
{
    if(argc != 1)
    {
	printf("Usage: %s \nFollowed by num_puzzle and the puzzle\n", argv[0]);
	return 1;
    }

    int numpuzzles;
    scanf("%d", &numpuzzles);

    unsigned int *** puzzles = malloc(sizeof(unsigned int **) * numpuzzles);
    int i;
    for(i=0; i<numpuzzles; i++)
    {
	puzzles[i] = malloc(sizeof(unsigned int *) * 9);
	int j;
	for(j=0; j<9; j++)
	{
	    puzzles[i][j] = malloc(sizeof(unsigned int) * 9);
	    int k;
	    for(k=0; k<9; k++)
	    {
		scanf("%u", &puzzles[i][j][k]);
	    }
	}
    }

// DEBUG
    printf("This is debugging\n");
    for(i=0; i<10; i++)
    {
	unsigned int value = make_singleton(i);
	printf("%u is singleton of %u, is_single = %u\n", value, from_singleton(value), is_singleton(value));
    }


    printf("Here is the original\n");
    print_puzzle_original(puzzles[0]);

    for(i=0; i<numpuzzles; i++)
    {
	solve_sudoku(puzzles[i]);
	free_puzzle_mem(puzzles[i]);
    }
    return 0;
}
