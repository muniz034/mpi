#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

void printarVetor(int rank, int *A, int n);
void OddEvenSort(int A[], int n);
void generateVector(int *vector, int m);
int compare(const void * a, const void * b);

int main(int argc, char **argv){
    int size;
    int rank;
    
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = atoi(argv[1]);
    int *A = (int *)malloc(sizeof(int) * n);

    if(rank == 0){
        srand(time(NULL));
        generateVector(A, n);
        // printarVetor(rank, A, n);
    }

    MPI_Scatter(A, n / size, MPI_INT, A, n / size, MPI_INT, 0, MPI_COMM_WORLD);

    n /= size;
    A = (int *)realloc(A, sizeof(int) * n);

    qsort(A, n, sizeof(int), compare);

    double time = MPI_Wtime();

    for(int i = 0; i < size; i++){
        if(i % 2 == 1){ // Odd
            if(rank % 2 == 1){ // Se o processo for HIGH (0,[1]) (2,[3])
                A = (int *)realloc(A, sizeof(int) * (n * 2));
                int *B = (int *)malloc(sizeof(int) * n);
                MPI_Recv(B, n, MPI_INT, rank - 1, 200, MPI_COMM_WORLD, &status);
                memcpy(&A[n], B, sizeof(int) * n);
                free(B);
                OddEvenSort(A, n * 2);
                MPI_Send(A, n, MPI_INT, rank - 1, 200, MPI_COMM_WORLD);
                memcpy(A, &A[n], sizeof(int) * n);
                A = (int *)realloc(A, sizeof(int) * n);
            } else { // Se o processo for LOW ([0],1) ([2],3)
                MPI_Send(A, n, MPI_INT, rank + 1, 200, MPI_COMM_WORLD);
                MPI_Recv(A, n, MPI_INT, rank + 1, 200, MPI_COMM_WORLD, &status);
            }
        } else if(rank > 0 && rank < size - 1) { // Even
            if(rank % 2 == 0){ // Se o processo for HIGH (1, [2])
                A = (int *)realloc(A, sizeof(int) * (n * 2));
                int *B = (int *)malloc(sizeof(int) * n);
                MPI_Recv(B, n, MPI_INT, rank - 1, 200, MPI_COMM_WORLD, &status);
                memcpy(&A[n], B, sizeof(int) * n);
                free(B);
                OddEvenSort(A, n * 2);
                MPI_Send(A, n, MPI_INT, rank - 1, 200, MPI_COMM_WORLD);
                memcpy(A, &A[n], sizeof(int) * n);
                A = (int *)realloc(A, sizeof(int) * n);
            } else { // Se o processo for LOW ([1], 2)
                MPI_Send(A, n, MPI_INT, rank + 1, 200, MPI_COMM_WORLD);
                MPI_Recv(A, n, MPI_INT, rank + 1, 200, MPI_COMM_WORLD, &status);
            }
        }
        // MPI_Barrier(MPI_COMM_WORLD);
    }

    if(rank == 0) A = (int *)realloc(A, sizeof(int) * (n * size));

    // MPI_Gather(A, n, MPI_INT, A, n * size, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank != 0){
        MPI_Send(A, n, MPI_INT, 0, 200, MPI_COMM_WORLD);
    } else {
        A = (int *)realloc(A, sizeof(int) * (n * size));
        for(int i = 1; i < size; i++) MPI_Recv(&A[i * n], n, MPI_INT, i, 200, MPI_COMM_WORLD, &status);
    }

    // if(rank == 0) printarVetor(rank, A, n * size);
    if(rank == 0) printf("end: %f\n", MPI_Wtime() - time);

    free(A);

    MPI_Finalize();

    return 0;
}

int compare(const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
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