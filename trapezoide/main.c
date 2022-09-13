#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

double f(double x){
    return x * x;
}

double calculaIntegral(double *limits, double deltaX){
    double soma = 0;
    soma += f(limits[0])/2.0;
    soma += f(limits[1])/2.0;

    for(double x = limits[0] + deltaX; x < limits[1]; x += deltaX) soma += f(x);
    
    return soma * deltaX;
}

int main(int argc, char **argv){
    int size;
    int rank;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int limitA = atoi(argv[1]);
    int limitB = atoi(argv[2]);

    int nTrapezoides = atoi(argv[3]);
    double deltaX = (double) (limitB - limitA)/nTrapezoides;
    double nTrapezoidesPorRank = nTrapezoides/size;

    double a = limitA + (rank * nTrapezoidesPorRank * deltaX);
    double b = a + (nTrapezoidesPorRank * deltaX);

    double limits[2] = { a, b };

    double integralParcial = calculaIntegral(limits, deltaX);
    double integral = integralParcial;

    if(rank != 0){
        MPI_Send(&integral, 1, MPI_DOUBLE, 0, 200, MPI_COMM_WORLD);
    } else {
        for(int i = 1; i < size; i++){
            MPI_Recv(&integralParcial, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            integral += integralParcial;
        }

        printf("INTEGRAL: %f\n", integral);
    }

    MPI_Finalize();

    return 0;
}