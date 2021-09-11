#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "empty_cell.h"

extern int Count_Chars_Grid(char **sudoku, int n, char character);
extern int Count_Char_Column(char** arr, int height, int column, char character);
extern int Count_Char(char* arr, int arr_len, char character);
extern int Count_Char_Block(int row, int column, char **sudoku, int block_size, char character);
int Modify_Pencil(char **sudoku, short ***sudoku_pencil_p, int n, int row, int col, char min_char);
int Mod_Pencil_Col(char **sudoku, short ***sudoku_pencil_p, int n, int col, char character, char min_char);
int Mod_Pencil_Row(char * sudoku_row, short **sudoku_pencil_row, int n, char character, char min_char);
int Mod_Pencil_Block(char **sudoku, short ***sudoku_pencil, int n, int row_corner, int col_corner, char character, char min_char);
short Only_One(short * arr, int n);
void Free_Pencil(int n, short ***sudoku_pencil);
int Check_Filled(char **sudoku, int n, char min_char);

int Only_In_Column(short ***sudoku_pencil, int n, int col, char character, char min_char);
int Find_Column_Exclusive(short ***sudoku_pencil,char ***sudoku_in, int n, char min_char);
int Only_In_Row(short ***sudoku_pencil, int n, int row, char character, char min_char);
int Find_Row_Exclusive(short ***sudoku_pencil, char ***sudoku_in, int n, char min_char);
int* Only_In_Block(short ***sudoku_pencil, int n, int row_corner, int col_corner, char character, char min_char);
int Find_Block_Exclusive(short ***sudoku_pencil, char ***sudoku_in, int n, char min_char);

int All_Locked_Cand_Claims(short ***sudoku_pencil, int n, char min_char);
int Locked_Cand_Claim_Row(short ***sudoku_pencil, int n, int row, char character, char min_char);
int Locked_Cand_Claim_Col(short ***sudoku_pencil, int n, int col, char character, char min_char);

int Hidden_Double_Row(short ***sudoku_pencil, int n, int row, char char1, char char2, char min_char);
int All_Hidden_Doubles(short ***sudoku_pencil, int n, char min_char);

void All_Nishio(short ***sudoku_pencil, char ***sudoku_in, int n, char min_char);

void Finish_Pencil_Marks(short *cell_pencil, int n, char min_char, char character);

extern void Print_Sudoku(int n, char **sudoku);

extern int Pre_Verify_Blocks(int n, char **sudoku);
extern int Verify_Rows_and_Columns(int n, char **sudoku, int print);

void Print_Array(short * arr, int n);

/* Modifies the given sudoku to be solved
 * 
 * sudoku_in :: a pointer to the sudoku
 * n :: the dimensions of the sudoku. every sudoku is nxn
 * min_char :: the minimum valued character in the sudoku
 * return :: 1 if it could solve it, 0 if not
 */
int Solve(char ***sudoku_in, int n, char min_char) {
    char ** sudoku = *sudoku_in;

    // setting up a 3d array which stores the possible answers for each one
    short *** sudoku_pencil;
    sudoku_pencil = malloc(n * sizeof(short**));
    for(int i = 0; i < n; i++){
        sudoku_pencil[i] = malloc(n * sizeof(short*));
        for(int j = 0; j < n; j++){
            //each array stores the 1s and 0s representing whether the cell could include the character that corresponds to the position in the array
            sudoku_pencil[i][j] = calloc(n, sizeof(short));
            if(sudoku[i][j] != EMPTY_CELL) {
                sudoku_pencil[i][j][sudoku[i][j] - min_char] = 1;
            } else {
                // marking everything as 1 on cells that are empty (space in them)
               
                for(int k = 0; k < n; k++){
                    sudoku_pencil[i][j][k] = 1;
                }
            }
        }
    }

    //continues going until teh board has been filled up
    while(Check_Filled(sudoku, n, min_char) == 0){
        int changed = 0; // keeps track of whether a dead end has been found

        //Print_Sudoku(n, sudoku);
        //printf("\n");
        
        // if a cell has only 1 possibly character, then the cell is filled in
        for(int row = 0; row < n; row++){
            for(int col = 0; col < n; col++){
                short loc = Only_One(sudoku_pencil[row][col], n);
                if(loc != -1){
                    if(sudoku[row][col] != (min_char + loc)){
                        changed = 1;
                    }
                    sudoku[row][col] = (min_char + loc);
                }
            }
        }

        // modifying the pencil marks to make sure the pencil marks align with what is possible in each cell
        for(int row = 0; row < n; row++){
            for(int col = 0; col < n; col++){
                changed = changed || Modify_Pencil(sudoku, sudoku_pencil, n, row, col, min_char);
            }
        }
        
        if(!Pre_Verify_Blocks(n, sudoku) || !Verify_Rows_and_Columns(n, sudoku, 0)){
            return 0;
        }
        changed = changed || Find_Row_Exclusive(sudoku_pencil, sudoku_in, n, min_char);
        changed = changed || Find_Column_Exclusive(sudoku_pencil, sudoku_in, n, min_char);
        changed = changed || Find_Block_Exclusive(sudoku_pencil, sudoku_in, n, min_char);  
        changed = changed || All_Locked_Cand_Claims(sudoku_pencil, n, min_char);
        changed = changed || All_Hidden_Doubles(sudoku_pencil, n, min_char);
            
        // if nothing else works, try nishio
        if(!changed){
            All_Nishio(sudoku_pencil, sudoku_in, n, min_char);
        }
        
        
    }

    Free_Pencil(n, sudoku_pencil);
    return 1;
}

void Copy_Sudoku(char ***sudoku_in_orig, char ***sudoku_in_copy, int n){
    char **sudoku_orig = *sudoku_in_orig;

    char **sudoku_copy = *sudoku_in_copy;

    sudoku_copy = malloc(n * sizeof(char *));
    for(int i = 0; i < n; i++){
        sudoku_copy[i] = malloc(n * sizeof(char));
        for(int j = 0; j < n; j++){
            sudoku_copy[i][j] = sudoku_orig[i][j];
        }
    }
}


extern void Free_Memory(int n, char **sudoku);
// return 1 if it works, 0 otherwise
int Nishio_Iter(char ***sudoku_in, int n, int row, int col, char character, char min_char){
    char** sudoku_copy;

    char **sudoku_orig = *sudoku_in;
    //printf("BEFORE MALLOC row %d col %d\n", row, col);
    //getchar();

    sudoku_copy = malloc(n * sizeof(char *));
    for(int i = 0; i < n; i++){
        sudoku_copy[i] = malloc(n * sizeof(char));
        for(int j = 0; j < n; j++){
            sudoku_copy[i][j] = sudoku_orig[i][j];
        }
    }

    sudoku_copy[row][col] = character;
    //Copy_Sudoku(sudoku_in, sudoku_copy_in, n);
    char ***sudoku_copy_in = &sudoku_copy;
    
    int solved = Solve(sudoku_copy_in, n, min_char);
    if(solved){
        Free_Memory(n, sudoku_orig);
        sudoku_orig = sudoku_copy;
    } else {
        Free_Memory(n, *sudoku_copy_in);
    }
    return solved;
}

// returns -1 if nothing works, otherwise returns character of what works
int Nishio_Cell(char*** sudoku_in, short*** sudoku_pencil, int n, int row, int col, char min_char){

    for(int i = 0; i < n; i++){
        if(sudoku_pencil[row][col][i] == 1){
            if(Nishio_Iter(sudoku_in, n, row, col, min_char + i, min_char)){  
                return min_char + i;
            }
        }
    }
    return -1;
}

// does Nishio on all squares where there are only 2 pencil marks
void All_Nishio(short ***sudoku_pencil, char ***sudoku_in, int n, char min_char){

    char **sudoku = *sudoku_in;
    for(int row = 0; row < n; row++){
        for(int col = 0; col < n; col++){
            int marks = 0;
            for(int i = 0; i < n; i++){
                marks = marks + sudoku_pencil[row][col][i];
            }
            if(marks == 2){
                int result = Nishio_Cell(sudoku_in, sudoku_pencil, n, row, col, min_char);
                if(result != -1){
                    sudoku[row][col] = result;
                    Finish_Pencil_Marks(sudoku_pencil[row][col], n, min_char, result);
                    return;
                }
            }
        }  
    }
}

//given a cell which has been completed, it erases all pencil marks but the one for the charaacter it was filled in with
void Finish_Pencil_Marks(short *cell_pencil, int n, char min_char, char character){
    for(int i = 0; i < n; i++){
        if(i + min_char == character){
            cell_pencil[i] = 1;
        } else {
            cell_pencil[i] = 0;
        }
    }
}

// if there is only 1 of a character in some column's pencil marks, the cell with the mark is 
// filled with the character and 1 is returned. if it can't find that, then 0 is returned
int Find_Row_Exclusive(short ***sudoku_pencil, char ***sudoku_in, int n, char min_char){
    int return_val = 0;
    char **sudoku = *sudoku_in;
    for(int row = 0; row < n; row++){
        for(char character = min_char; character < min_char + n; character++){
            int col = Only_In_Row(sudoku_pencil, n, row, character, min_char);
            if(col != -1){
                if(sudoku[row][col] != character){
                    return_val = 1;
                }
                sudoku[row][col] = character;
                Finish_Pencil_Marks(sudoku_pencil[row][col], n, min_char, character);
            }
        }
    }
    return return_val;
}

int Only_In_Row(short ***sudoku_pencil, int n, int row, char character, char min_char){
    int char_index = character - min_char;
    int col_result = -1;
    for(int col = 0; col < n; col++){
        if(sudoku_pencil[row][col][char_index] == 1){
            if(col_result != -1){
                return -1;
            }
            col_result = col;
        }
    }
    return col_result;
}

// if there is only 1 of a character in some column's pencil marks, the cell with the mark is 
// filled with the character and 1 is returned. if it can't find that, then 0 is returned
int Find_Column_Exclusive(short ***sudoku_pencil, char ***sudoku_in, int n, char min_char){
    int return_val = 0;
    char **sudoku = *sudoku_in;
    for(int col = 0; col < n; col++){
        for(char character = min_char; character < min_char + n; character++){
            int row = Only_In_Column(sudoku_pencil, n, col, character, min_char);
            if(row != -1){
                if(sudoku[row][col] != character){
                    return_val = 1;
                }
                sudoku[row][col] = character;
                Finish_Pencil_Marks(sudoku_pencil[row][col], n, min_char, character);
            }
        }
    }
    return return_val;
}

// If there is only 1 of a character in the pencil marks of a column, it finds it, returning the row where the mark is
int Only_In_Column(short ***sudoku_pencil, int n, int col, char character, char min_char){
    int char_index = character - min_char;
    int row_result = -1; // stores which was the latest found row that had the only char matching character in the hints of the column
    for(int row = 0; row < n; row++){
        if(sudoku_pencil[row][col][char_index] == 1){
            if(row_result != -1){
                return -1; //if there are two spots with the character as a possibility, return -1
            }
            row_result = row;
        }
    }
    return row_result;
}

// if there is only 1 of a character in some block's pencil marks, the cell with the mark is filled
// with the character and 1 is returned. if it can't find that, then 0 is returned
int Find_Block_Exclusive(short ***sudoku_pencil, char ***sudoku_in, int n, char min_char){
    int return_val = 0;    
    char **sudoku = *sudoku_in;
    for(int row_corner; row_corner < n; row_corner += sqrt(n)){
        for(int col_corner; col_corner < n; col_corner += sqrt(n)){
            for(char character = min_char; character < min_char + n; character++){
                int* result = Only_In_Block(sudoku_pencil, n, row_corner, col_corner, character, min_char);
                if(result[0] != -1){
                    if(sudoku[result[0]][result[1]] != character){
                        return_val = 1;
                    }
                    sudoku[result[0]][result[1]] = character;
                    Finish_Pencil_Marks(sudoku_pencil[result[0]][result[1]], n, min_char, character);
                }
                free(result);
            }
        }
    }
    return return_val;
}

// If there is only 1 of a character in the pencil marks of a block, it finds it, returning a pointer to the row and column
int* Only_In_Block(short ***sudoku_pencil, int n, int row_corner, int col_corner, char character, char min_char){
    int *result = malloc(sizeof(int) * 2); // first stored here is row, then col
    result[0] = -1;
    result[1] = -1;

    int char_index = character - min_char;

    for(int row = row_corner; row < row_corner + sqrt(n); row++){
        for(int col = col_corner; col < col_corner + sqrt(n); col++){
            if(sudoku_pencil[row][col][char_index] == 1){
                if(result[0] != -1){
                    return result;
                } else {
                    result[0] = row;
                    result[1] = col;
                }
            }
        }
    }
    return result;
}

// does locked candidate claims http://hodoku.sourceforge.net/en/tech_intersections.php
int All_Locked_Cand_Claims(short ***sudoku_pencil, int n, char min_char){
    int return_val = 0;
    for(int row = 0; row < n; row++){
        for(char character = min_char; character < min_char + n; character++){
            if(Locked_Cand_Claim_Row(sudoku_pencil, n, row, character, min_char)){
                return_val = 1;
            }
        }
    }
    
    for(int col = 0; col < n; col++){
        for(char character = min_char; character < min_char + n; character++){
            if(Locked_Cand_Claim_Col(sudoku_pencil, n, col, character, min_char)){
                return_val = 1;
            }
        }
    }
    return return_val;
}

// takes a row and character. if it finds that the character in the pencil marks of the row is
// confined to one block, then it eliminates the other pencil marks of the character in the row
int Locked_Cand_Claim_Row(short ***sudoku_pencil, int n, int row, char character, char min_char){

    int char_index = character - min_char;

    // figures out if it the character is only confined to the pencil marks of one of the blocks in the row
    int block = -1; // blocks are numbered left to right from 0
    for(int col = 0; col < n; col++){
        if(sudoku_pencil[row][col][char_index] == 1){
            int curr_block = col/sqrt(n);
            if(curr_block != block && block != -1){
                return 0;
                // this means  another blockhas already been discovered different from the current block that has pencil marks with the character
            } else {
                block = curr_block;
            }
        }
    }

    if(block == -1){
        return 0;
    }

    int return_val = 0;
    int block_row_corn = ((int) (row / sqrt(n))) * sqrt(n); // the row of the corner of the block
    int block_col_corn = block * sqrt(n);
    for(int curr_row = block_row_corn; curr_row < block_row_corn + sqrt(n); curr_row++){
        if(row != curr_row){
            for(int col = block_col_corn; col < block_col_corn + sqrt(n); col++){
                if(sudoku_pencil[curr_row][col][char_index] == 1){
                    return_val = 1;
                }
                sudoku_pencil[curr_row][col][char_index] = 0;
            }
        }
    }
    return return_val;
}

int Locked_Cand_Claim_Col(short ***sudoku_pencil, int n, int col, char character, char min_char){
    int char_index = character - min_char;

    int block = -1;
    for(int row = 0; row < n; row++){
        if(sudoku_pencil[row][col][char_index] == 1){
            int curr_block = row/sqrt(n);
            if(curr_block != block && block != -1){
                return 0;
            } else {
                block = curr_block;
            }
        }
    }

    if(block == -1){
        return 0;
    }
    
    int return_val = 0;
    int block_col_corn = ((int) (col / sqrt(n))) * sqrt(n); // the row of the corner of the block
    int block_row_corn = block * sqrt(n);
    for(int curr_col = block_col_corn; curr_col < block_col_corn + sqrt(n); curr_col++){
        if(col != curr_col){
            for(int row = block_row_corn; row < block_row_corn + sqrt(n); row++){
                if(sudoku_pencil[row][curr_col][char_index] == 1){
                    return_val = 1;
                }
                sudoku_pencil[row][curr_col][char_index] = 0;
            }
        }
    }
    return return_val;
}

int All_Hidden_Doubles(short ***sudoku_pencil, int n, char min_char){
    int return_val = 0;
    for(int row = 0; row < n; row++){
        for(char char1 = min_char; char1 < min_char + n; char1++){
            for(char char2 = min_char; char2 < min_char + n; char2++){
                if(Hidden_Double_Row(sudoku_pencil, n, row, char1, char2, min_char)){
                    return return_val;
                }
            }
        }   
    }

    return return_val;
}

int Hidden_Double_Row(short ***sudoku_pencil, int n, int row, char char1, char char2, char min_char){
    int count = 0;
    
    int char1_index = char1 - min_char;
    int char2_index = char2 = min_char;

    int col1 = -1;
    int col2 = -1;

    for(int col = 0; col < n; col++){
        int char1_present = (sudoku_pencil[row][col][char1_index] == 1);
        int char2_present = (sudoku_pencil[row][col][char2_index] == 1);
        if(char1_present && char2_present){
            count++;
            if(col1 == -1){
                col1 = col;
            } else {
                col2 = col;
            }
        } else if (char1_present || char2_present){
            return 0;
        }
    }
    
    if(count != 2){
        return 0;
    }


    int return_val = 0;

    for(int i = 0; i < n; i++){
        if(i != char1_index && i != char2_index){
            if(sudoku_pencil[row][col1][char1_index] == 1 || sudoku_pencil[row][col2][char1_index] == 1){
                return_val = 1;
            }
            sudoku_pencil[row][col1][char1_index] = 0;
            sudoku_pencil[row][col2][char1_index] = 0;
        }
    }
    return return_val;
}

/*
 * Checks if the sudoku is filled
 */
int Check_Filled(char **sudoku, int n, char min_char){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(sudoku[i][j] == EMPTY_CELL){
                return 0;
            }
        }
    }
    return 1;
}

/* Tests if there is exactly one 1 in the array and returns its index
 * 
 * arr :: the array to be tested
 * n :: the length of the array
 * return :: the index of the position of the 1 if there is only one 1. otherwise -1
 */
short Only_One(short * arr, int n){
    short count = 0;
    short location = -1;
    for(int i = 0; i < n; i++){
        if(arr[i] == 1){
            count += 1;
            location = i;
        }
    }
    if(count != 1){

        return -1;
    } else {
        return location;
    }
}

/* Looks at a particular filled cell and changes the pencil marks in empty cells accordingly
 * 
 * sudoku :: the puzzle board
 * sudoku_pencil :: the pencil marks on the sudoku
 * n :: the dimensions of the sudoku. every sudoku is nxn
 * row :: the row the particular filled cell
 * col :: the column of the particular filled cell
 * min_char :: the minimum valued character allowed on the board (beside spaces representing empty spots)
 */
int Modify_Pencil(char **sudoku, short ***sudoku_pencil, int n, int row, int col, char min_char){
    int return_val = 0;

    int block_corner_row = (row / ((int)sqrt(n))) * ((int) sqrt(n));
    int block_corner_col = (col / ((int)sqrt(n))) * ((int) sqrt(n));
    if(sudoku[row][col] != EMPTY_CELL){
        return_val = return_val || Mod_Pencil_Col(sudoku, sudoku_pencil, n, col, sudoku[row][col], min_char); 
        return_val = return_val || Mod_Pencil_Row(sudoku[row], sudoku_pencil[row], n, sudoku[row][col], min_char);
        return_val = return_val || Mod_Pencil_Block(sudoku, sudoku_pencil, n, block_corner_row, block_corner_col, sudoku[row][col], min_char);
    }

    return return_val;
}

/* Eliminates a particular cell from the pencil marks in a column
 * 
 * sudoku :: the puzzle board
 * sudoku_pencil :: the pencil marks on the sudoku
 * n :: the dimensions of the sudoku. every sudoku is nxn
 * col :: the column
 * character :: the character to be eliminated from the pencil marks
 * min_char :: the minimum valued character allowed on the board (beside spaces representing empty spots)
 */
int Mod_Pencil_Col(char **sudoku, short ***sudoku_pencil, int n, int col, char character, char min_char){
    int return_val = 0;
    for(int row = 0; row < n; row++) {
        if(sudoku[row][col] != character){
            if(sudoku_pencil[row][col][character - min_char] == 1){
                return_val = 1;
            }
            sudoku_pencil[row][col][character - min_char] = 0;
        }
    }
    return return_val;
}

/* Eliminates a particular cell from the pencil marks in a row
 * 
 * sudoku_row :: the puzzle board row
 * sudoku_pencil_row :: the pencil marks on the sudoku row
 * n :: the dimensions of the sudoku. every sudoku is nxn
 * character :: the character to be eliminated from the pencil marks
 * min_char :: the minimum valued character allowed on the board (beside spaces representing empty spots)
 */
int Mod_Pencil_Row(char * sudoku_row, short **sudoku_pencil_row, int n, char character, char min_char) {
    int return_val = 0;
    for(int col = 0; col < n; col++){
        if(sudoku_row[col] != character){
            if(sudoku_pencil_row[col][character - min_char] == 1){
                return_val = 1;
            }
            sudoku_pencil_row[col][character - min_char] = 0;
        }
    }

    return return_val;
}

/* Eliminates a particular cell from the pencil marks in a block
 * 
 * sudoku :: the puzzle board
 * sudoku_pencil :: the pencil marks on the sudoku
 * n :: the dimensions of the sudoku. every sudoku is nxn
 * row_corner :: the row of the top left corner cell of the block
 * col_corenr :: the column of the top left corner cell of the block
 * character :: the character to be eliminated from the pencil marks
 * min_char :: the minimum valued character allowed on the board (beside spaces representing empty spots)
 */
int Mod_Pencil_Block(char **sudoku, short ***sudoku_pencil, int n, int row_corner, int col_corner, char character, char min_char){
    int return_val = 0;
    for(int row = row_corner; row < row_corner + sqrt(n); row++){
        for(int col = col_corner; col < col_corner + sqrt(n); col++){
            if(sudoku[row][col] != character){
                if(sudoku_pencil[row][col][character - min_char] == 1){
                    return_val = 1;
                }
                sudoku_pencil[row][col][character - min_char] = 0;
            }
        }
    }

    return return_val;
}

void Free_Pencil(int n, short ***sudoku_pencil){
    for(int row = 0; row < n; row++) {
        for(int col = 0; col < n; col++) {
            free(sudoku_pencil[row][col]);
        }
        free(sudoku_pencil[row]);
    }
    free(sudoku_pencil);
}

void Print_Array(short * arr, int n){
    for(int i = 0; i < n; i++){
        printf("%d", arr[i]);
    }
    printf("\n");
}