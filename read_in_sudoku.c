
// sudoku_valid.txt AND sudoku_valid_incomplete.txt COMES FROM FIRST EXAMPLE ON http://www.sudoku-download.net/files/Solution_60_Sudokus_4x4_Easy.pdf on http://www.sudoku-download.net/sudoku_4x4.php
// sudoku_invalid.txt IS A MODIFIED VERSION OF sudoku_valid.txt
// sudoku_difficult COMES FROM FIRST EXAMPLE ON http://www.sudoku-download.net/files/60_Sudokus_4x4_Difficult.pdf on http://www.sudoku-download.net/sudoku_4x4.php
// sudoku_9x9_difficult.txt COMES FROM THE FIRST EXAMPEL ON http://www.sudoku-download.net/files/60_Sudokus_Pattern_Difficult.pdf on http://www.sudoku-download.net/sudoku_9x9.php

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "empty_cell.h"

int Count_Char(char* arr, int arr_len, char character);
int Count_Char_Column(char** arr, int height, int column, char character);
int Count_Chars_Grid(char **sudoku, int n, char character);
int Check_Block_Duplicates(int row, int column, char **sudoku, int block_size);
int Count_Char_Block(int row, int column, char **sudoku, int block_size, char character);

/* This function takes the name of the file containing the sudoku
 * and reads in the data to the the sudoku variable in main.  
 *
 * n :: the dimensions of the board.  Note all boards tested will be square (i.e. nxn)
 * filename :: the name of the file to read in
 * sudoku_in :: a pointer to the sudoku variable in main
 */
void Read_Sudoku_File(char *filename, char ***sudoku_in, int *n) {

    // there are many approaches that will work to read in the puzzle data
    // you will need to do at least the following
    // 1) open the file 
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        *sudoku_in = NULL;
        return;
    }

    // 2) read in the text from the file
    // use a temporary char array called text to read in the data as it is unclear how long the rows are currently
    char *text = malloc((126 - 33 + 1) * (126 - 33 + 1) * sizeof(char));
    char character;
    for(int i = 0; (character = getc(fp)) != EOF; i++){
        *(text + i) = character;
    }

    // 3) figure out the dimensions of the puzzle (n)
    //   3a) note the puzzle will always be square - we will not test any input that is not nxn.
    //   3b) we will only test with puzzles using valid symbols 
    //   3c) a valid symbol has ascii code 33 to 126
    for(int i = 0;; i++){
        if(*(text + i)== '\n'){
            *n = i;
            break;
        }
    }

    // 4) reserve memory for the sudoku_square with malloc. This requires 2 steps
    //   4a) reserve an array of pointers to the rows
    *sudoku_in = malloc((*n) * sizeof(char*));
    //   4b) reserve an array of characters for each row
    for(int i = 0; i < (*n); i++){
        *(*sudoku_in + i) = malloc((*n) * sizeof(char));
    }

    // 5) fill in the sudoku_square data structure 
    int curr = 0;
    for(int i = 0; i < (*n); i++){
        for(int j = 0; j < (*n); j++){
            *((*(*sudoku_in + i)) + j) = *(text + curr);
            curr++;
        }
        curr++;
    }
    // 6) close the file
    fclose(fp);
    // free the memory allocated for the data to be read into
    free(text);
    return;
}

/* This function checks to see that exactly n symbols are used and that each symbol is used exactly n times.
 * Valid symbols include charcters with ascii codes 33 to 126 (inclusive)
 * Google "man ascii" to pull up the table
 *
 * n :: the dimensions of the puzzle - all puzzles will be square (i.e. nxn)
 * sudoku :: the puzzle data structure
 * return :: 0 if the alphabet is invalid and 1 if the alphabet is valid
 */
int Verify_Alphabet(int n, char **sudoku) {
    if (sudoku == NULL) {
        printf("Verify_Alphabet - sudoku is NULL\n"); 
        return 0;
    }

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            // checks whether the character in row i and column j appears exactly n times
            if(Count_Chars_Grid(sudoku, n, *(*(sudoku + i) + j)) != n){
                return 0;
            }
        }
    }
    return 1;
}

/* This function counts when a character occurs in a 2d array
 * 
 * arr :: array of characters to test
 * n :: the dimensions of the puzzle
 * character :: character to search for
 * return :: the number of occurences of the character
 */
int Count_Chars_Grid(char **sudoku, int n, char character){
    int count = 0;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            // checks whether the character at row i and column j is the searched for character. if so, the count is incremented
            if(*(*(sudoku + i) + j) == character){
                count += 1;
            }
        }
    }
    return count;
}


/* This function counts when a character occurs in a column in a 2d array
 * 
 * arr :: array of characters to test
 * arr_len :: the height of the column
 * column :: column in the array to test
 * character :: character to search for in array
 * return :: the number of occurences of the character in the column
 */
int Count_Char_Column(char** arr, int height, int column, char character){
    int count = 0;
    for(int i = 0; i < height; i++){
        if(*(*(arr + i) + column) == character){
            count += 1;
        }
    }
    return count;
}

/* This function counts when a character occurs in an array
 * 
 * arr :: array of characters to test
 * arr_len :: the length of the array
 * character :: character to search for in array
 * return :: the number of occurences of the character
 */
int Count_Char(char* arr, int arr_len, char character){
    int count = 0;
    for(int i = 0; i < arr_len; i++){
        if(*(arr + i)== character){
            count += 1;
        }
    }
    return count;
}

/* This function calls free to return all memory used by the sudoku puzzle
 * note you will have n+1 calls to free where n is the size of the puzzle.
 *
 * n :: the dimensions of the puzzle - all puzzles will be square (i.e. nxn)
 * sudoku :: the puzzle data structure
 */ 
void Free_Memory(int n, char **sudoku) {
    for(int i = 0; i < n; i++){
        free(*(sudoku + i));
    }
    
    free(sudoku);
    return;
}

/* This function tests if blocks of charcters in the square have 1 of each character
 * Blocks here is used in a similar sense to suduko (i.e. the 3x3 groups of 9 cells) but dynamically sized
 * 
 * n :: the dimensions of the puzzle - all puzzles will be square (i.e. nxn)
 * sudoku :: the puzzle data structure
 * return :: 0 if it is impossible for square blocks to each have n characters in them or a block contains a duplicate, 1 otherwise
 */
int Verify_Blocks(int n, char **sudoku){
    int block_size = sqrt(n);
    if(n % block_size){
        return 0;
    }

    for(int block_row = 0; block_row < n / block_size; block_row++){
        for(int block_column = 0; block_column < n / block_size; block_column++){
            if(Check_Block_Duplicates(block_row * block_size, block_column * block_size, sudoku, block_size) == 0){
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
int Check_Block_Duplicates(int row, int column, char **sudoku, int block_size){
    for(int i = row; i < row + block_size; i++){
        for(int j = column; j < column + block_size; j++){
            if(Count_Char_Block(row, column, sudoku, block_size, *(*(sudoku + i) + j)) != 1){
                return 0;
            }
        }
    }
    return 1;
}

int Count_Char_Block(int row, int column, char **sudoku, int block_size, char character){
    int count = 0;
    for(int i = row; i < row + block_size; i++){
        for(int j = column; j < column + block_size; j++){
            if(*(*(sudoku + i) + j) == character){
                count += 1;
            }
        }
    }
    return count;
}
