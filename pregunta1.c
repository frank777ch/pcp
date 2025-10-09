#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 500

void prodmv_paralelo(double a[N], double c[N], double B[N][N]) {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < N; j++) {
            sum += B[i][j] * c[j];
        }
        a[i] = sum;
    }
}

int main() {
    static double matriz_B[N][N];
    static double vector_c[N];
    static double vector_a[N];

    for (int i = 0; i < N; i++) {
        vector_c[i] = (double)(i + 1);
        for (int j = 0; j < N; j++) {
            matriz_B[i][j] = (double)(i + j);
        }
    }

    printf("Realizando la multiplicación Matriz-Vector en paralelo...\n");
    
    double t_inicio = omp_get_wtime();

    prodmv_paralelo(vector_a, vector_c, matriz_B);

    double t_fin = omp_get_wtime();

    printf("Calculo finalizado en %f segundos.\n", t_fin - t_inicio);

    printf("\nAlgunos resultados:\n");
    printf("a[0] = %f\n", vector_a[0]);
    printf("a[%d] = %f\n", N-1, vector_a[N-1]);

    return 0;
}