#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 2000

/**
 * Función que implementa el producto matriz-vector en paralelo.
 * "a": Vector de salida (resultado).
 * "B": Matriz de entrada.
 * "c": Vector de entrada.
 */
void prodmvp(double a[N], double c[N], double B[N][N]) {
    // Declaración de variables para los bucles y la suma.
    int i, j;
    double sum;

    #pragma omp parallel for private(j, sum)
    for (i = 0; i < N; i++) {
        // Cada hilo inicializa su propia copia de 'sum' a 0.
        sum = 0.0;
        
        // Cada hilo ejecuta este bucle interno para las filas 'i' que le fueron asignadas.
        for (j = 0; j < N; j++) {
            sum += B[i][j] * c[j];
        }
        
        // No hay condición de carrera aquí, porque aunque 'a' es un arreglo compartido,
        // cada hilo escribe en una posición diferente y única (a[i]).
        a[i] = sum;
    }
}


int main() {
    // Se asigna memoria estáticamente para la matriz y los vectores.
    static double B[N][N], c[N], a[N];
    int i, j;

    // Se inicializa la semilla para generar números aleatorios.
    srand(time(NULL));

    // Se llenan la matriz B y el vector c con valores aleatorios entre 0 y 9.
    for (i = 0; i < N; i++) {
        c[i] = rand() % 10;
        for (j = 0; j < N; j++) {
            B[i][j] = rand() % 10;
        }
    }
    
    // --- Medición de tiempo ---
    double start_time = omp_get_wtime();

    // Se llama a la función paralela para realizar el cálculo.
    prodmvp(a, c, B);

    double end_time = omp_get_wtime();

    // Se imprime el tiempo de ejecución.
    printf("Calculo completado en %f segundos.\n", end_time - start_time);

    // Se imprime una pequeña porción del resultado para verificar que se ejecutó.
    printf("\nPrimeros 5 elementos del vector resultado 'a':\n");
    for(i = 0; i < 5; i++) {
        printf("a[%d] = %f\n", i, a[i]);
    }

    return 0;
}