#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "empty_cell.h"

extern void Read_Sudoku_File(char *filename, char ***sudoku, int *n);
extern int Count_Chars_Grid(char **sudoku, int n, char character);
extern int Count_Char_Column(char** arr, int height, int column, char character);
extern int Count_Char(char* arr, int arr_len, char character);
extern int Count_Char_Block(int row, int column, char **sudoku, int block_size, char character);
int Pre_Check_Block_Duplicates(int row, int column, char **sudoku, int block_size);
int Verify_Rows_and_Columns(int n, char **sudoku, int print);

/* This function checks to see that no more than n valid symbols are used and that each symbol is used no more than n times. 
 * It also checks that the characters in the sudoku are within the proper range
 * Valid symbols include charcters with ascii characters in teh specified range
 * Google "man ascii" to pull up the table
 *
 * n :: the dimensions of the puzzle - all puzzles will be square (i.e. nxn)
 * sudoku :: the puzzle data structure
 * return :: 0 if the alphabet is invalid and 1 if the alphabet is valid
 */
int Pre_Verify_Alphabet(int n, char **sudoku, char min_char, char max_char) {
    if(max_char - min_char + 1 != n){
        printf("Character range not proper for dimensions\n");
        return 0;
    }

    if(EMPTY_CELL <= max_char && EMPTY_CELL >= min_char){
        printf("Character range cannot include a space\n");
        return 0;
    }

    if (sudoku == NULL) {
        printf("Verify_Alphabet - sudoku is NULL\n"); 
        return 0;
    }

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            // checks whether the character in row i and column j appears no more than n times and is within range or a space
            if(((sudoku[i][j] >= min_char && sudoku[i][j] <= max_char) && Count_Chars_Grid(sudoku, n, *(*(sudoku + i) + j)) <= n) || sudoku[i][j] == EMPTY_CELL){
                ;
            } else {
                return 0;
            }
        }
    }
    return 1;
}

/* This function tests if blocks of charcters in the square have no more than 1 of each character (excluding spaces)
 * Blocks here is used in a similar sense to suduko (i.e. the 3x3 groups of 9 cells) but dynamically sized
 * 
 * n :: the dimensions of the puzzle - all puzzles will be square (i.e. nxn)
 * sudoku :: the puzzle data structure
 * return :: 0 if it is impossible for square blocks to each have n characters in them or a block contains a duplicate, 1 otherwise
 */
int Pre_Verify_Blocks(int n, char **sudoku){
    int block_size = sqrt(n);
    if(n % block_size){
        return 0;
    }

    for(int block_row = 0; block_row < n / block_size; block_row++){
        for(int block_column = 0; block_column < n / block_size; block_column++){
            if(Pre_Check_Block_Duplicates(block_row * block_size, block_column * block_size, sudoku, block_size) == 0){
                return 0;
            }
        }
    }
    return 1;
}

/* This function tests if a given block has duplicate characters
 * 
 * row :: the row of the top left cell in the block
 * column :: the column of the top left cell in the block
 * sudoku :: the puzzle data structure
 * block_size :: the dimensions of each block, specifically side length
 */
int Pre_Check_Block_Duplicates(int row, int column, char **sudoku, int block_size){
    for(int i = row; i < row + block_size; i++){
        for(int j = column; j < column + block_size; j++){
            if(sudoku[i][j] != EMPTY_CELL && Count_Char_Block(row, column, sudoku, block_size, *(*(sudoku + i) + j)) != 1){
                return 0;
            }
        }
    }
    return 1;
}

/* This function verifies that no symbol is used twice in a row or column beside spaces.
 * It prints an error message alerting the user which rows or columns have duplicate symbols
 * Test all rows first then test all columns.
 * Error messages have been included for you.
 * 
 * n :: the dimensions of the puzzle - all puzzles will be square (i.e. nxn)
 * sudoku :: the puzzle data structure
 * return :: 0 if the any row or column contains a duplicate symbol otherwise return 1
 */
int Pre_Verify_Rows_and_Columns(int n, char **sudoku){
    return Verify_Rows_and_Columns(n, sudoku, 1);
}

/* This function verifies that no symbol is used twice in a row or column beside spaces.
 * It prints an error message alerting the user which rows or columns have duplicate symbols
 * Test all rows first then test all columns.
 * Error messages have been included for you.
 * 
 * n :: the dimensions of the puzzle - all puzzles will be square (i.e. nxn)
 * sudoku :: the puzzle data structure
 * return :: 0 if the any row or column contains a duplicate symbol otherwise return 1
 */
int Verify_Rows_and_Columns(int n, char **sudoku, int print){
    if (sudoku == NULL) {
        if(print){
            printf("Verify_Rows_and_Columns - sudoku is NULL\n"); 
        }
        return 0;
    }
    
    // verify each row
    for(int rowNum = 0; rowNum < n; rowNum++){
        char *row = *(sudoku + rowNum);
        for(int colNum = 0; colNum < n; colNum++){
            if(sudoku[rowNum][colNum] != EMPTY_CELL && Count_Char(row, n,*(*(sudoku + rowNum) + colNum)) > 1){
                if(print){
                    printf("Error in row %d\n", rowNum);
                }
                return 0;
            }
        }
    }

    // verify each column
    for(int colNum = 0; colNum < n; colNum++){
        for(int rowNum = 0; rowNum < n; rowNum++){
            if(sudoku[rowNum][colNum] != EMPTY_CELL && Count_Char_Column(sudoku, n, colNum, *(*(sudoku + rowNum) + colNum)) > 1){
                if(print){
                    printf("Error in column %d\n", colNum);
                }
                return 0;
            }
        }
    }
        // printf("Error in row %d\n", row);
        // printf("Error in column %d\n", col);
    
    return 1;
}

