#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpi.h"
#include "omp.h"

double f(double x){
    return x * x;
}

float calculaIntegral(float a, float b, float deltaX, int nTrapezoidesPorRank);

int main(int argc, char **argv){
    int size;
    int rank;
    int threadLevel;
    MPI_Status status;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &threadLevel);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int limitA = atoi(argv[1]);
    int limitB = atoi(argv[2]);
    int nTrapezoides = atoi(argv[3]);
    int nThreads = atoi(argv[4]);

    double deltaX = (double) (limitB - limitA)/nTrapezoides;

    int nTrapezoidesPorRank = nTrapezoides/size;
    int nTrapezoidesPorThread = nTrapezoidesPorRank/nThreads;

    double a = limitA + (rank * nTrapezoidesPorRank * deltaX);
    double b = (rank == size - 1) ? limitB : a + (nTrapezoidesPorRank * deltaX);

    // Balanceamento de carga
    if(nTrapezoides % size > 0){
        if(rank < (nTrapezoides % size)){
            nTrapezoidesPorRank++;
            a = (limitA + (rank * nTrapezoidesPorRank * deltaX));
            b = (a + nTrapezoidesPorRank * deltaX);
        } else {
            a = (limitA + rank * nTrapezoidesPorRank * deltaX) + ((nTrapezoides % size) * deltaX);
            b += (rank == size - 1) ? 0 : ((nTrapezoides % size) * deltaX);
        }
    }
    
    printf("%d >>> %f %f %d\n", rank, a, b, nTrapezoidesPorRank);

    double somaParcial = 0;
    double soma = 0;

    #pragma omp parallel shared(soma) private(somaParcial) num_threads(nThreads)
    {

        double threadA = a + (omp_get_thread_num() * nTrapezoidesPorThread * deltaX);
        double threadB = threadA + (nTrapezoidesPorThread * deltaX);

        somaParcial = calculaIntegral(threadA, threadB, deltaX, nTrapezoidesPorThread);

        #pragma omp barrier

        if(rank != 0){
            MPI_Send(&somaParcial, 1, MPI_DOUBLE, 0, 50, MPI_COMM_WORLD);
        } else {
            #pragma omp atomic
                soma += somaParcial;
        }
        
        if(rank == 0){
            #pragma omp barrier
        }

        if(rank == 0 && omp_get_thread_num() == 0){
            double temp;

            for(int i = 0; i < (size - 1) * nThreads; i++){
                temp = 0;
                MPI_Recv(&temp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 50, MPI_COMM_WORLD, &status);
                soma += temp;
            }

            printf("INTEGRAL: %f\n", soma);
        }
    }
    
    MPI_Finalize();

    return 0;
}

float calculaIntegral(float a, float b, float deltaX, int nTrapezoidesPorRank){
    float soma = (f(a) + f(b))/2.0;
    float x = a;

    for(int i = 0; i < nTrapezoidesPorRank; i++){
        x += deltaX;
        soma += f(x);
    }

    return soma * deltaX;
}