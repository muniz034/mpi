#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

void printarVetor(int rank, int *A, int n);
void OddEvenSort(int A[], int n);
void generateVector(int *vector, int m);

int main(int argc, char **argv){
    int size;
    int rank;
    
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = atoi(argv[1]);
    int *A = (int *)malloc(sizeof(int) * n);
    
    srand(time(NULL));
    generateVector(A, n);

    OddEvenSort(A, n);

    free(A);

    MPI_Finalize();

    return 0;
}

void OddEvenSort(int A[], int n){
    for(int i = 1; i <= n; i++){
        if(i % 2 == 0){ // Even
            for(int j = 0; j < (n/2); j++){
                if(A[2 * j] > A[2 * j + 1]){
                    int temp = A[2 * j];
                    A[2 * j] = A[2 * j + 1];
                    A[2 * j + 1] = temp;
                }
            }
        } 

        if(i % 2 == 1){ // Odd
            for(int j = 0; j < (n/2) - 1; j++){
                if(A[2 * j + 1] > A[2 * j + 2]){
                    int temp = A[2 * j + 1];
                    A[2 * j + 1] = A[2 * j + 2];
                    A[2 * j + 2] = temp;
                }
            }
        }
    }
}

void generateVector(int *vector, int m){
    for(int i = 0; i < m; i++) vector[i] = rand() % m;
}

void printarVetor(int rank, int *A, int n){
    printf("Processo %d\n", rank);
    for(int i = 0; i < n; i++) printf("%d; ", A[i]);
    printf("\n");
}