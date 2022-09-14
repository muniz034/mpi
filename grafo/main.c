#include <stdio.h>
#include <string.h>
#include "mpi.h"

int matriz[6][6] = {    {0, 1, 1, 0, 0, 0},
                        {1, 0, 1, 1, 1, 0},
                        {1, 1, 0, 0, 0, 1},
                        {0, 1, 0, 0, 0, 0},
                        {0, 1, 0, 0, 0, 0},
                        {0, 0, 1, 0, 0, 0},
                    };

int numeroDeTarefas = 6;

int contaNumeroDeVizinhos(int rank){
    int contador = 0;

    for(int i = 0; i < numeroDeTarefas; i++){
        if(matriz[rank][i] == 1) contador++;
    }

    return contador;
}

int main(int argc, char **argv){
    int size;
    int rank;
    char message[100] = "Oi!";
    
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int numeroDeVizinhos = contaNumeroDeVizinhos(rank);

    if(rank != 0){
        MPI_Recv(message, strlen(message) + 1, MPI_CHAR, MPI_ANY_SOURCE, 50, MPI_COMM_WORLD, &status);
        printf("[%d] Recebendo mensagem de: %d\n", rank, status.MPI_SOURCE);
        numeroDeVizinhos--;
    }

    for(int i = 0; i < numeroDeTarefas; i++){
        if(matriz[rank][i] == 1){
            printf("[%d] Enviando mensagem para: %d\n", rank, i);
            MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, 50, MPI_COMM_WORLD);
        }
    }

    for(int i = 0; i < numeroDeVizinhos; i++){
        MPI_Recv(message, strlen(message) + 1, MPI_CHAR, MPI_ANY_SOURCE, 50, MPI_COMM_WORLD, &status);
        printf("[%d] Recebendo mensagem de: %d\n", rank, status.MPI_SOURCE);
    }

    MPI_Finalize();
}