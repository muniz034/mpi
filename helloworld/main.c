#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char **argv){
    int size;
    int rank;
    char str[100];

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank != 0){
        sprintf(str, "Hello World! from %d", rank);
        MPI_Send(str, strlen(str) + 1, MPI_CHAR, 0, 200, MPI_COMM_WORLD);
    } else {
        for(int i = 1; i < size; i++){
            MPI_Recv(&str, 100, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Pacote recebido de %d: %s\n", status.MPI_SOURCE, str);
        }
    }

    MPI_Finalize();

    return 0;
}