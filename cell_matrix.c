#include "cell_matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

//sleep for msec
static int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

static Cell_matrix alloc_matrix(int width, int height)
{
    const int arr_size = height * sizeof(Cell_matrix*);
    const int row_size = width * sizeof(Cell_matrix);

    Cell_matrix cm;
    int** state = malloc(arr_size);
    int** next_state = malloc(arr_size);

    int row;
    for (row = 0; row < height; row++)
    {
        state[row] = malloc(row_size);
        next_state[row] = malloc(row_size);
    }
    
    cm.state = state;
    cm.next_state = next_state;
    cm.width = width;
    cm.height = height;

    return cm;
}

Cell_matrix init_random_cell_matrix(int width, int height)
{
    srand(time(0));
    Cell_matrix cm = alloc_matrix(width, height);

    int row, col;
    for (row = 0; row < height; row++)
        for(col = 0; col < width; col++)
            cm.state[row][col] = cm.next_state[row][col] = rand() % 2;

    return cm;
}

Cell_matrix init_cell_matrix(FILE* file)
{
    int width = 0;
    int height = 0;

    const int MAX_WIDTH = 10000;
    char buff[MAX_WIDTH];
    while(fgets(buff, MAX_WIDTH, file) != NULL)
    {
        if (width == 0)
        {
            width = strlen(buff) - 1; // omitting the endline character
        }
        else if (strlen(buff) - 1 != width)
        {
            fprintf(stderr, "Zła szerokosć linii.\n");
            fclose(file);
            exit(1);
        }
        height++;
    }
    rewind(file);

    Cell_matrix cm = alloc_matrix(width, height);

    int row = 0;
    while(fgets(buff, MAX_WIDTH, file) != NULL)
    {
        int col;
        for (col = 0; col < width; col++)
            cm.state[row][col] = cm.next_state[row][col] = buff[col] - '0';
        row++;
    }

    return cm;
}

static int get_cell_value(const Cell_matrix* cm, int row, int col)
{
    if (row > 0 && row < cm->height && col > 0 && col < cm->width)
    {
        return cm->state[row][col];
    }
    return 0;
}

/*
    Martwa komórka, która ma dokładnie 3 żywych sąsiadów, staje się żywa w następnej jednostce czasu (rodzi się)
    Żywa komórka z 2 albo 3 żywymi sąsiadami pozostaje nadal żywa; przy innej liczbie sąsiadów umiera (z „samotności” albo „zatłoczenia”)
*/

void evolve(Cell_matrix* cm)
{
    int living_neighbors = 0;
    
    int row, col;
    for (row = 0; row < cm->height; row++)
        for (col = 0; col < cm->width; col++)
        {
            living_neighbors =
            get_cell_value(cm, row - 1, col - 1)
            + get_cell_value(cm, row - 1, col)
            + get_cell_value(cm, row - 1, col + 1)
            + get_cell_value(cm, row, col - 1)
            + get_cell_value(cm, row, col + 1)
            + get_cell_value(cm, row + 1, col - 1)
            + get_cell_value(cm, row + 1, col)
            + get_cell_value(cm, row + 1, col + 1);

            if (cm->state[row][col] == 0 && living_neighbors == 3)
                cm->next_state[row][col] = 1;
            if (cm->state[row][col] == 1 && (living_neighbors < 2 || living_neighbors > 3))
                cm->next_state[row][col] = 0;
        }

    for (row = 0; row < cm->height; row++)
        for (col = 0; col < cm->width; col++)
        {
            cm->state[row][col] = cm->next_state[row][col];
        }
}

void print_cell_matrix(Cell_matrix cm)
{
    int row, col;
    for (row = 0; row < cm.height; row++)
    {
        for (col = 0; col < cm.width; col++)
        {
            const char* block = "█";
            if (cm.state[row][col] == 1)
                printf("%s", block);
            else
                printf(" ");
        }
        printf("\n");
    }

    msleep(125);
    system("clear");
}

void delete_cell_matrix(Cell_matrix* cm)
{
    int row;
    for (row = 0; row < cm->height; row++)
    {
        free(cm->state[row]);
        free(cm->next_state[row]);
    }

    free(cm->state);
    free(cm->next_state);
}