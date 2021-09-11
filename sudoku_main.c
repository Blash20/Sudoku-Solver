#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "empty_cell.h"

extern void Read_Sudoku_File(char *filename, char ***sudoku, int *n);
extern void Free_Memory(int n, char **sudoku);
extern int Pre_Verify_Alphabet(int n, char **sudoku, char min_char, char max_char);
extern int Pre_Verify_Blocks(int n, char **sudoku);
extern int Pre_Verify_Rows_and_Columns(int n, char **sudoku);

extern int Solve(char ***sudoku_in, int n, char min_char);


void Print_Sudoku(int n, char **sudoku);

int main(int argc, char *argv[] ) {
  // Verify command line parameters
    if(argc != 2 + 2 && argc != 2 + 2 + 1){
        printf("expected usage: %s <file_name> <min_char> <max_char> <empty_cell_char(optional)>", argv[0]); 
        exit(1);
    } else if (argc == 2 + 2 + 1){
        EMPTY_CELL = argv[4][0];
    } else {
        EMPTY_CELL = ' ';
    }

    char min_char = argv[2][0];
    char max_char = argv[3][0];

    /* Declare variables
     * n ::  the dimensions of the square 
     * sudoku :: represents the puzzle.  This variable is a pointer to a pointer.  
     * You can think of it as an array that contains pointers to arrays of characters for each row.
     */
    int n=0;
    char **sudoku = NULL;
    int pre_alphabet_valid, pre_rows_cols_valid, pre_blocks_valid;

    // Read the sudoku file
    Read_Sudoku_File(argv[1], &sudoku, &n);
    // Print the sudoku;
    Print_Sudoku(n, sudoku);

    printf("\n");

    // Verify Alphabet
    if ((pre_alphabet_valid = Pre_Verify_Alphabet(n, sudoku, min_char, max_char))) 
        printf("Alphabet is Valid\n"); 
    else printf("Alphabet is Invalid\n");

    // Verify each letter used once per row and column
    if ((pre_rows_cols_valid = Pre_Verify_Rows_and_Columns(n, sudoku))) 
        printf("Rows and Columns do not contain duplicate entries\n"); 
    else printf("Rows and Columns contain duplicate entries\n");

    if(pre_blocks_valid = Pre_Verify_Blocks(n, sudoku))
        printf("Blocks do not contain duplicate entries\n");
    else printf("Blocks contain duplicate entries\n");

    if (pre_alphabet_valid && pre_rows_cols_valid && pre_blocks_valid) {
        printf("Sudoku is valid\n");
        Solve(&sudoku, n, min_char);
        printf("\nSolved:\n");
        Print_Sudoku(n, sudoku);
    } else printf("Sudoku is invalid\n");
    
    // Free memory
    Free_Memory(n, sudoku);
    
    return 0;
}