#include <stdio.h>

typedef struct Cell_matrix
{
    int** state;
    int** next_state;
    int width;
    int height;
}Cell_matrix;

Cell_matrix init_random_cell_matrix(int width, int height);
Cell_matrix init_cell_matrix(FILE* file);
void evolve(Cell_matrix*);
void print_cell_matrix(const Cell_matrix*);
void delete_cell_matrix(Cell_matrix*);
