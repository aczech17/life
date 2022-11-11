#include "cell_matrix.h"
#include <stdlib.h>
#include <signal.h>

// break the infinite loop for ctrl c
volatile sig_atomic_t stop;
void inthand(int intnum)
{
    stop = 1;
}

int main(int argc, char** argv)
{
    signal(SIGINT, inthand);

    Cell_matrix cm;
    int width, height;

    if (argc < 2)
    {
        fprintf(stderr, "Podaj nazwę pliku, albo szerokość i wysokość.\n");
        return 1;
    }

    if (argc >= 3) // width and height
    {
        width = atof(argv[1]);
        height = atof(argv[2]);
        cm = init_random_cell_matrix(width, height);
    }
    else // argc == 2
    {
        FILE* in = fopen(argv[1], "r");
        if (in == NULL)
        {
            fprintf(stderr, "Nie można otworzyć pliku %s.\n", argv[1]);
            return 1;
        }
        cm = init_cell_matrix(in);
        fclose(in);
    }

    while(!stop)
    {
        print_cell_matrix(&cm);
        evolve(&cm);
    }

    delete_cell_matrix(&cm);
    return 0;
}
