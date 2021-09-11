# Sudoku-Solver
Solves sudoku of any nxn size where n is a square number.

If the given incomplete sudoku has a mistake in it or is not of the correct format, the program will throw and error and not attempt to solve.

## How to use:
The format taken by the program is a text file with the characters of the sudoku written out. By default, empty cells are written as spaces (although this can be changed with a parameter).
The characters used to fill the sudoku must occupy a contiguous range of encoding values which cannot include the character used for empty cell. The number of them must equal the side length of the square.

Use the following format to run: 
    
  ./sudoku.exe <file_name> <min_char> <max_char> <empty_cell_char(optional)>
 
Here is an example of usage:

![Capture](https://user-images.githubusercontent.com/23222973/132960369-01b8d51c-841a-456c-9b68-a5fc03ee648d.PNG)



## Credit and assistance:

This started as an extension of a school project which just tested the validity of latin squares (similar to sudoku but only accounts for rows and columnns, not blocks)
but did not solve them.

Therefore some of the function prototypes in read_in_sudoku.c, sudoku_pre_check.c, and sudoku_print.c were created by my professor. I filled the function bodies.

This was uploaded with the full permission of my professor. Their name has not been included here at their request to avoid students finding this and using it.
