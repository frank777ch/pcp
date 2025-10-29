#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/**
 * Calcula a qué bin pertenece un dato y aumenta el contador de ese bin.
 * data: El valor flotante a clasificar.
 * bin_counts: Arreglo que almacena la cuenta de cada bin.
 * bin_count: El número total de bins.
 * min_meas: El valor mínimo del rango del histograma.
 * max_meas: El valor máximo del rango del histograma.
 */
void Find_bin(float data, int* bin_counts, int bin_count, float min_meas, float max_meas) {

    // Se calcula el ancho de cada contenedor dividiendo el rango total por el número de bins.
    float bin_width = (max_meas - min_meas) / bin_count;
    
    // Se determina el índice del bin para el dato. Se resta el mínimo para normalizar
    // y luego se divide por el ancho del bin.
    int bin = (int)((data - min_meas) / bin_width);
    
    // Verificación de límites: si el dato es menor que el mínimo o mayor/igual que el máximo,
    // se ajusta para que caiga en el primer o último bin respectivamente.
    if (bin < 0) {
        bin = 0;
    } else if (bin >= bin_count) {
        bin = bin_count - 1;
    }
    
    // Se incrementa el contador del bin correspondiente.
    bin_counts[bin]++;
}

/**
 * Imprime una representación gráfica del histograma en la consola.
 * bin_counts: Arreglo con las cuentas finales de cada bin.
 * bin_count: Número total de bins.
 * min_meas: Valor mínimo del rango.
 * max_meas: Valor máximo del rango.
 */
void Print_hist(int bin_counts[], int bin_count, float min_meas, float max_meas) {
    float bin_width = (max_meas - min_meas) / bin_count;
    float bin_start = min_meas;

    printf("\n============== Histograma Final ==============\n");
    // Se itera sobre cada bin para imprimirlo.
    for (int i = 0; i < bin_count; i++) {
        // Se imprime el rango del bin actual, ej: [0.00, 1.00).
        printf("[%6.2f, %6.2f) | ", bin_start, bin_start + bin_width);
        
        // Se imprime un '#' por cada elemento en el bin para la visualización.
        for (int j = 0; j < bin_counts[i]; j++) {
            printf("#");
        }
        
        // Se muestra el conteo numérico al final de la barra.
        printf(" (%d)\n", bin_counts[i]);
        
        // Se actualiza el inicio del siguiente bin.
        bin_start += bin_width;
    }
    printf("============================================\n");
}


// --- Función Principal ---
int main(int argc, char** argv) {
    // Declaración de variables de MPI
    int rank; // Identificador (ID) del proceso actual.
    int p;    // Número total de procesos.

    // Variables del histograma
    float* data = NULL;        // Puntero al arreglo de datos (solo usado en rank 0).
    int data_count;            // Número total de datos.
    float min_meas, max_meas;  // Rango de los datos.
    int bin_count;             // Número de contenedores del histograma.

    // --- 1. Inicialización de MPI ---
    // Inicializa el entorno MPI. Debe ser la primera función MPI llamada.
    MPI_Init(&argc, &argv);
    // Obtiene el rank (ID) del proceso que está ejecutando este código.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Obtiene el número total de procesos en el comunicador global.
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // --- 2. Lectura y Configuración (Solo en Proceso 0) ---
    if (rank == 0) {
        // Se definen los datos de entrada directamente en el código para este ejemplo.
        float initial_data[] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
        data_count = sizeof(initial_data) / sizeof(float); // Se calcula el número de datos.
        min_meas = 0.0;   // Valor mínimo del histograma.
        max_meas = 5.0;   // Valor máximo del histograma.
        bin_count = 5;    // Número de bins.

        // Se asigna memoria para el arreglo de datos.
        data = (float*)malloc(data_count * sizeof(float));
        // Se copian los datos iniciales al arreglo dinámico.
        for(int i = 0; i < data_count; i++) {
            data[i] = initial_data[i];
        }

        // Se imprime la configuración inicial.
        printf("Proceso 0: Configuración -> %d datos, rango [%.2f, %.2f], %d bins.\n", data_count, min_meas, max_meas, bin_count);
    }

    // --- 3. Distribución de Parámetros ---
    // MPI_Bcast envía una copia de una variable desde el proceso root (0) a todos los demás.
    // Aquí, todos los procesos necesitan conocer los parámetros del histograma.
    MPI_Bcast(&data_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&min_meas, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&max_meas, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bin_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // --- 4. Distribución de Datos ---
    // Se prepara la distribución de datos. Esto es necesario para dividir el trabajo.
    int* sendcounts = (int*)malloc(p * sizeof(int)); // Arreglo que dice cuántos elementos enviar a cada proceso.
    int* displs = (int*)malloc(p * sizeof(int));     // Arreglo que dice el desplazamiento inicial para los datos de cada proceso.
    int offset = 0;

    // Se calcula cuántos datos le tocan a cada proceso.
    for (int i = 0; i < p; i++) {
        sendcounts[i] = data_count / p;
        if (i < data_count % p) { // Si no es divisible, los primeros 'resto' procesos reciben uno más.
            sendcounts[i]++;
        }
        displs[i] = offset;
        offset += sendcounts[i];
    }
    
    // Cada proceso reserva memoria para su parte de los datos.
    float* local_data = (float*)malloc(sendcounts[rank] * sizeof(float));

    // MPI_Scatterv distribuye los datos del arreglo 'data' (en rank 0) a todos los 'local_data'.
    // Es "v" (vector) porque el número de elementos para cada proceso puede variar.
    MPI_Scatterv(data, sendcounts, displs, MPI_FLOAT, local_data, sendcounts[rank], MPI_FLOAT, 0, MPI_COMM_WORLD);

    // --- 5. Cálculo en Paralelo ---
    // Cada proceso crea y inicializa en cero su propio arreglo para el histograma local.
    // calloc inicializa la memoria a cero, lo cual es perfecto para contadores.
    int* local_bin_counts = (int*)calloc(bin_count, sizeof(int));
    
    // Cada proceso itera sobre SUS datos locales y calcula su histograma.
    for (int i = 0; i < sendcounts[rank]; i++) {
        Find_bin(local_data[i], local_bin_counts, bin_count, min_meas, max_meas);
    }
    
    // --- 6. Agregación de Resultados ---
    int* global_bin_counts = NULL;
    if (rank == 0) {
        // Solo el proceso 0 necesita memoria para el histograma final/global.
        global_bin_counts = (int*)calloc(bin_count, sizeof(int));
    }

    // MPI_Reduce combina los datos de todos los procesos y almacena el resultado en el proceso root (0).
    // 'local_bin_counts' es la entrada de cada proceso.
    // 'global_bin_counts' es la salida en el proceso 0.
    // 'bin_count' es el número de elementos a reducir.
    // 'MPI_SUM' es la operación de reducción.
    MPI_Reduce(local_bin_counts, global_bin_counts, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // --- 7. Impresión de Resultados  ---
    if (rank == 0) {
        // El proceso 0, que ahora tiene el histograma completo, lo imprime.
        Print_hist(global_bin_counts, bin_count, min_meas, max_meas);
        
        // Libera la memoria que solo el proceso 0 utilizó.
        free(global_bin_counts);
        free(data);
    }

    // --- 8. Limpieza y Finalización ---
    // Todos los procesos liberan la memoria que utilizaron.
    free(sendcounts);
    free(displs);
    free(local_data);
    free(local_bin_counts);

    // Finaliza el entorno MPI. Debe ser la última función MPI llamada.
    MPI_Finalize();
    
    return 0;
}