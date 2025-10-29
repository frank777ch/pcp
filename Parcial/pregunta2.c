#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N 1024

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, p;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtiene el ID del proceso actual.
    MPI_Comm_size(MPI_COMM_WORLD, &p);    // Obtiene el número total de procesos.

    // El problema asume que N es divisible por p. Añadimos una comprobación
    // para que el programa termine de forma controlada si no se cumple.
    if (N % p != 0) {
        if (rank == 0) {
            printf("Error: N (%d) no es divisible por el numero de procesos p (%d).\n", N, p);
        }
        MPI_Finalize();
        return 1; // Termina con un código de error.
    }

    double (*A)[N] = NULL; // Matriz completa, solo P0 la usará. Puntero a un arreglo de N doubles.
    double *v = NULL, *x = NULL; // Vectores completos, solo P0 los usará.
    
    // El número de filas que cada proceso manejará.
    int local_rows = N / p;

    if (rank == 0) {
        printf("Iniciando calculo con N=%d en %d procesos (%d filas por proceso).\n", N, p, local_rows);
        // P0 reserva memoria para la matriz y los vectores completos.
        // Usamos esta sintaxis para asegurar que la memoria de la matriz sea contigua,
        // lo cual es requerido por MPI_Scatter.
        A = malloc(sizeof(double[N][N]));
        v = malloc(N * sizeof(double));
        x = malloc(N * sizeof(double));

        // P0 inicializa la matriz A y el vector v con datos aleatorios.
        srand(time(NULL));
        for (int i = 0; i < N; i++) {
            v[i] = rand() % 10;
            for (int j = 0; j < N; j++) {
                A[i][j] = rand() % 10;
            }
        }
    } else {
        // Los demás procesos también necesitan memoria para el vector 'v',
        // ya que lo recibirán completo a través de Broadcast.
        v = malloc(N * sizeof(double));
    }

    // Cada proceso reserva memoria para SU bloque de filas de A (`local_A`)
    // y para SU porción del vector resultado (`local_x`).
    double (*local_A)[N] = malloc(sizeof(double[local_rows][N]));
    double *local_x = malloc(local_rows * sizeof(double));

    // (a) Distribuir la matriz A por bloques de filas desde P0 a todos los procesos.
    // - A: El buffer de envío en el proceso root (P0).
    // - local_rows * N: El número de elementos a ENVIAR a cada proceso.
    // - local_A: El buffer de recepción en cada proceso.
    // - local_rows * N: El número de elementos a RECIBIR por cada proceso.
    // - 0: El ID del proceso root (el que envía).
    MPI_Scatter(A, local_rows * N, MPI_DOUBLE,
                local_A, local_rows * N, MPI_DOUBLE,
                0, MPI_COMM_WORLD);
    
    // (b) Enviar el vector v completo desde P0 a todos los demás procesos.
    // - v: El buffer que contiene los datos. P0 envía, los demás reciben en él.
    // - N: El número de elementos a transmitir.
    // - 0: El ID del proceso root (el que hace el broadcast).
    MPI_Bcast(v, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Cada proceso ahora tiene la información que necesita.
    // Se realiza la multiplicación matriz-vector para las filas locales.
    for (int i = 0; i < local_rows; i++) {
        local_x[i] = 0.0; // Inicializar el elemento del resultado.
        for (int j = 0; j < N; j++) {
            local_x[i] += local_A[i][j] * v[j];
        }
    }

    // P0 recolecta los resultados parciales (`local_x`) de todos los procesos
    // y los almacena en el vector final `x`.
    // - local_x: El buffer de envío de cada proceso.
    // - local_rows: El número de elementos que cada proceso envía.
    // - x: El buffer de recepción en el proceso root (P0).
    // - local_rows: El número de elementos que P0 recibe DE CADA proceso.
    // - 0: El ID del proceso root (el que recolecta).
    MPI_Gather(local_x, local_rows, MPI_DOUBLE,
               x, local_rows, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Calculo completado.\n");
        printf("\nPrimeros 5 elementos del vector resultado 'x':\n");
        for (int i = 0; i < 5; i++) {
            printf("x[%d] = %f\n", i, x[i]);
        }
        // P0 libera la memoria de los datos globales.
        free(A);
        free(x);
    }
    
    // Todos los procesos liberan la memoria que utilizaron.
    free(v);
    free(local_A);
    free(local_x);
    
    MPI_Finalize();
    return 0;
}