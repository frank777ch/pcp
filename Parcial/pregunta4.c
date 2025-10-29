#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

// Estas variables son globales para que sean accesibles por TODOS los hilos.
// Esto es la esencia de la memoria compartida.
int* global_bin_counts;    // Arreglo para el histograma final.
float* data;               // Arreglo con los datos de entrada.
int data_count;
int bin_count;
float min_meas, max_meas;

// Declaración del Mutex. Esto servirá para proteger el acceso
// a 'global_bin_counts'.
pthread_mutex_t hist_mutex;

/**
 * Estructura para pasar argumentos a cada hilo.
 * Cada hilo necesita saber qué porción del trabajo le corresponde.
 */
typedef struct {
    int thread_id;     // ID del hilo (0 a NUM_THREADS-1).
    int start_index;   // Índice del arreglo de datos donde empieza su trabajo.
    int end_index;     // Índice donde termina su trabajo.
} thread_args_t;

/**
 * La función que ejecutará cada hilo trabajador.
 * args: Un puntero a la estructura de argumentos del hilo.
 */
void* thread_histogram_task(void* args) {
    // Se convierte el puntero void* a nuestro tipo de estructura de argumentos.
    thread_args_t* my_args = (thread_args_t*)args;

    // Se calcula el ancho de cada bin, igual que en la versión MPI.
    float bin_width = (max_meas - min_meas) / bin_count;

    // Cada hilo itera ÚNICAMENTE sobre su porción asignada del arreglo 'data'.
    for (int i = my_args->start_index; i < my_args->end_index; i++) {
        // Se determina el índice del bin para el dato actual.
        int bin = (int)((data[i] - min_meas) / bin_width);
        
        // Verificación de límites.
        if (bin < 0) {
            bin = 0;
        } else if (bin >= bin_count) {
            bin = bin_count - 1;
        }

        // El hilo intenta bloquear el mutex. Si otro hilo ya lo tiene,
        // este hilo se quedará esperando aquí hasta que se libere.
        pthread_mutex_lock(&hist_mutex);

        // Esta es la operación crítica. Solo un hilo a la vez puede ejecutar esta línea.
        // Esto previene la condición de carrera.
        global_bin_counts[bin]++;

        // El hilo libera el mutex, permitiendo que otro hilo que esté esperando
        // pueda entrar a su sección crítica.
        pthread_mutex_unlock(&hist_mutex);
    }
    
    // Se libera la memoria de los argumentos, que fue asignada en el main.
    free(my_args);
    
    // Los hilos deben retornar un puntero void*. NULL es común cuando no se retorna nada específico.
    pthread_exit(NULL);
    return NULL;
}

// Se reutiliza la misma función de impresión del ejercicio anterior.
void Print_hist(int counts[], int count, float min_val, float max_val) {
    float width = (max_val - min_val) / count;
    float start = min_val;
    printf("\n============== Histograma Final ==============\n");
    for (int i = 0; i < count; i++) {
        printf("[%6.2f, %6.2f) | ", start, start + width);
        for (int j = 0; j < counts[i]; j++) {
            printf("#");
        }
        printf(" (%d)\n", counts[i]);
        start += width;
    }
    printf("============================================\n");
}

int main(void) {
    // Se declaran los datos de entrada y parámetros, igual que en la versión MPI.
    float initial_data[] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
    data_count = sizeof(initial_data) / sizeof(float);
    min_meas = 0.0;
    max_meas = 5.0;
    bin_count = 5;

    // Se asigna memoria para los arreglos globales.
    data = (float*)malloc(data_count * sizeof(float));
    for(int i = 0; i < data_count; i++) data[i] = initial_data[i];
    // calloc inicializa la memoria a 0, perfecto para nuestros contadores.
    global_bin_counts = (int*)calloc(bin_count, sizeof(int));
    
    printf("Hilo Principal: Configuracion -> %d datos, %d bins, %d hilos.\n", data_count, bin_count, NUM_THREADS);

    // Se declara un arreglo para almacenar los identificadores de los hilos.
    pthread_t threads[NUM_THREADS];
    
    // Se inicializa el mutex. El segundo argumento NULL indica atributos por defecto.
    // Esta función debe llamarse ANTES de que cualquier hilo intente usar el mutex.
    pthread_mutex_init(&hist_mutex, NULL);

    // Se calcula cuántos datos procesará cada hilo.
    int chunk_size = data_count / NUM_THREADS;
    int remainder = data_count % NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        // Se crea dinámicamente una estructura de argumentos para CADA hilo.
        // Es importante que cada hilo tenga su propia copia de los argumentos.
        thread_args_t* args = (thread_args_t*)malloc(sizeof(thread_args_t));
        args->thread_id = i;
        // Se calcula el rango de datos para este hilo.
        args->start_index = i * chunk_size + (i < remainder ? i : remainder);
        args->end_index = args->start_index + chunk_size + (i < remainder ? 1 : 0);

        // Se crea el hilo.
        // - threads[i]: donde se guardará el ID del hilo.
        // - NULL: atributos por defecto.
        // - thread_histogram_task: la función que el hilo ejecutará.
        // - args: el puntero a los argumentos para esa función.
        pthread_create(&threads[i], NULL, thread_histogram_task, (void*)args);
    }

    // --- Sincronización ---
    // El hilo principal espera a que TODOS los hilos trabajadores terminen.
    // pthread_join es una operación de bloqueo. El main no continuará hasta
    // que el hilo 'threads[i]' haya finalizado.
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // --- Finalización ---
    // Una vez que todos los hilos han terminado, el mutex ya no es necesario.
    // Se destruye para liberar los recursos del sistema asociados a él.
    pthread_mutex_destroy(&hist_mutex);

    // Se imprime el resultado final. En este punto, 'global_bin_counts'
    // contiene la suma de todo el trabajo realizado por los hilos.
    Print_hist(global_bin_counts, bin_count, min_meas, max_meas);

    // Se libera toda la memoria dinámica asignada.
    free(data);
    free(global_bin_counts);

    return 0;
}