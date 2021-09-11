// Do not turn this file in.
// Do all of your work in 
// sudoku_square_functions.c!!!
// make sure your functions 
// work with the original 
// sudoku_square_main.c file!


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// you will write definitions for these functions in sudoku_square_functions.c
extern void Read_Sudoku_File(char *filename, char ***sudoku, int *n);
extern int Verify_Alphabet(int n, char **sudoku);
extern int Verify_Rows_and_Columns(int n, char **sudoku);
extern void Free_Memory(int n, char **sudoku);
extern int Verify_Blocks(int n, char **sudoku);


// use this function to verify that you have correctly read the sudoku
void Print_Sudoku(int n, char **sudoku) {
    if (sudoku == NULL) {
        printf("Print_Square - sudoku is NULL\n"); 
        return;
    }
    //printf("%d",n);
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++){
            printf("%c",sudoku[i][j]);
        }
        printf("\n");
    }
}

