#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 8

int main(int argc, char* argv[]) {
    int rank, p;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (N % p != 0) {
        if (rank == 0) {
            printf("Error: N (%d) debe ser divisible por el número de procesos (%d).\\n", N, p);
        }
        MPI_Finalize();
        return 1;
    }

    int rows_per_proc = N / p;
    double *A, *v, *x;
    
    double local_A[rows_per_proc][N];
    double local_x[rows_per_proc];

    double local_v[N];

    if (rank == 0) {
        printf("Ejecutando con %d procesos.\\n", p);
        printf("Cada proceso manejará %d filas.\\n\\n", rows_per_proc);

        A = (double *)malloc(N * N * sizeof(double));
        v = (double *)malloc(N * sizeof(double));
        x = (double *)malloc(N * sizeof(double));

        for (int i = 0; i < N; i++) {
            v[i] = (double)(i + 1);
            for (int j = 0; j < N; j++) {
                A[i*N + j] = (double)(i + 1);
            }
        }
    }

    MPI_Scatter(A, rows_per_proc * N, MPI_DOUBLE,
                local_A, rows_per_proc * N, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    MPI_Bcast(local_v, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < rows_per_proc; i++) {
        local_x[i] = 0.0;
        for (int j = 0; j < N; j++) {
            local_x[i] += local_A[i][j] * local_v[j];
        }
    }

    MPI_Gather(local_x, rows_per_proc, MPI_DOUBLE,
               x, rows_per_proc, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("--- Resultado Final ---\\n");
        for (int i = 0; i < N; i++) {
            printf("x[%d] = %f\\n", i, x[i]);
        }

        free(A);
        free(v);
        free(x);
    }

    MPI_Finalize();
    return 0;
}