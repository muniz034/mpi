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
    int pai = -1;
    int vizinhos;
    char message[100] = "Oi!";
    
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int numeroDeVizinhos = contaNumeroDeVizinhos(rank);

    // Quem não for o root espera uma mensagem
    if(rank != 0){
        MPI_Recv(message, strlen(message) + 1, MPI_CHAR, MPI_ANY_SOURCE, 50, MPI_COMM_WORLD, &status);
        printf("[%d] Recebendo mensagem de: %d\n", rank, status.MPI_SOURCE);
        // Atribui valor do pai como o id do processo caso não tenha
        if(pai == -1){
            pai = status.MPI_SOURCE;
            printf("[%d] Meu pai é: %d\n", rank, status.MPI_SOURCE);
        }
        numeroDeVizinhos--;
    }

    // Envio da mensagem para os nós vizinhos
    for(int i = 0; i < numeroDeTarefas; i++){
        if(matriz[rank][i] == 1 && i != pai){
            // printf("[%d] Enviando mensagem para o filho: %d\n", rank, i);
            MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, 50, MPI_COMM_WORLD);
        }
    }

    // Recebimento da confirmação dos filhos
    for(int i = 0; i < numeroDeVizinhos; i++){
        MPI_Recv(message, strlen(message) + 1, MPI_CHAR, MPI_ANY_SOURCE, 50, MPI_COMM_WORLD, &status);
        // printf("[%d] Recebendo confirmação do filho: %d\n", rank, status.MPI_SOURCE);
    }

    // Quem não for root, envia uma confirmação para o pai depois de receber as confirmações dos filhos
    if(rank != 0){
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, pai, 50, MPI_COMM_WORLD);
        // printf("[%d] Enviando confirmação para o pai: %d\n", rank, pai);
    }

    MPI_Finalize();
}