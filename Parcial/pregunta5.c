#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define DATA_COUNT 20
#define BIN_COUNT 5

float data[DATA_COUNT] = {
    1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3,
    4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9
};
int bin_counts[BIN_COUNT] = {0};
const float MIN_MEAS = 0.0;
const float MAX_MEAS = 5.0;

pthread_mutex_t bin_mutexes[BIN_COUNT];

typedef struct {
    long thread_id;
} ThreadData;

int find_bin(float value) {
    if (value < MIN_MEAS || value > MAX_MEAS) return -1;
    float bin_width = (MAX_MEAS - MIN_MEAS) / BIN_COUNT;
    int bin = (int)((value - MIN_MEAS) / bin_width);
    if (bin == BIN_COUNT) bin--;
    return bin;
}

void* worker_thread_function(void* arg) {
    ThreadData* t_data = (ThreadData*)arg;
    long thread_id = t_data->thread_id;

    int items_per_thread = DATA_COUNT / NUM_THREADS;
    int start_index = thread_id * items_per_thread;
    int end_index = start_index + items_per_thread;

    if (thread_id == NUM_THREADS - 1) {
        end_index = DATA_COUNT;
    }

    for (int i = start_index; i < end_index; i++) {
        int bin = find_bin(data[i]);
        if (bin != -1) {

            pthread_mutex_lock(&bin_mutexes[bin]);
            
            bin_counts[bin]++;
            
            pthread_mutex_unlock(&bin_mutexes[bin]);
        }
    }

    pthread_exit(NULL);
}

void print_histogram() {
    printf("\n--- Histograma Final ---\n");
    float bin_width = (MAX_MEAS - MIN_MEAS) / BIN_COUNT;
    for (int i = 0; i < BIN_COUNT; i++) {
        float bin_start = MIN_MEAS + i * bin_width;
        float bin_end = bin_start + bin_width;
        printf("Bin %2d [%.2f - %.2f]: ", i, bin_start, bin_end);
        for (int j = 0; j < bin_counts[i]; j++) {
            printf("*");
        }
        printf(" (%d)\n", bin_counts[i]);
    }
    printf("------------------------\n");
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < BIN_COUNT; i++) {
        pthread_mutex_init(&bin_mutexes[i], NULL);
    }

    // Paso 2: Crear los hilos trabajadores.
    printf("Creando %d hilos trabajadores...\n", NUM_THREADS);
    for (long i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        // La función pthread_create inicia un nuevo hilo que ejecutará 'worker_thread_function'.
        int rc = pthread_create(&threads[i], NULL, worker_thread_function, (void*)&thread_data[i]);
        if (rc) { // Verificar si hubo un error en la creación.
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Paso 3: Esperar a que todos los hilos terminen su trabajo.
    // El hilo principal se bloquea en pthread_join hasta que el hilo especificado finalice.
    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Todos los hilos han terminado.\n");

    // Paso 4: Imprimir el resultado final.
    // Esto es seguro porque ya sabemos que todos los hilos han terminado.
    print_histogram();

    // Paso 5: Liberar los recursos de los mutex.
    for (int i = 0; i < BIN_COUNT; i++) {
        pthread_mutex_destroy(&bin_mutexes[i]);
    }

    return 0;
}