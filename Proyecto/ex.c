#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PI 3.14159265358979323846

double generarNormalAleatorio(){
    static int flag = 0;
    static double valorGuardado;

    // retorna el valor guardado de "z2"

    if (flag) {
        flag = 0;
        return valorGuardado;
    }

    double u1, u2;

    // coloca los valores en el rango (0,1) sin que el u1 sea 0

    do {
        u1 = (double)rand() / (double)RAND_MAX;
    } while (u1 == 0.0);

    u2 = (double)rand() / (double)RAND_MAX;

    // se aplica la fórmula de Box-Muller

    double z1 = sqrt(-2.0 * log(u1)) * cos(2.0 * PI * u2);
    // z2
    valorGuardado = sqrt(-2.0 * log(u1)) * sin(2.0 * PI * u2);

    flag = 1;

    // retorna el valor de z1

    return z1;
}

double MontecarloCall(
    const long numeroSimulaciones, // Número de simulaciones
    const double S,             // Precio inicial de la acción (S_0)
    const double K,             // Precio de ejercicio (Strike price)
    const double r,             // Tasa de interés libre de riesgo
    const double T,             // Tiempo hasta el vencimiento (en años)
    const double sigma)         // Volatilidad de la acción
{
    double suma_payoffs = 0.0;

    for (long i = 0; i < numeroSimulaciones; ++i) {
        
        double Z = generarNormalAleatorio();

        // formula de GBM
        double S_T = S * exp((r - 0.5 * sigma * sigma) * T + sigma * sqrt(T) * Z);

        // beneficio (payoff) para una opción Call
        double payoff = fmax(0.0, S_T - K);

        // acumula el payoff
        suma_payoffs += payoff;
    }

    // calcula el promedio de los payoffs
    double promedio_payoff = suma_payoffs / numeroSimulaciones;

    // descuenta el promedio a valor presente
    return promedio_payoff * exp(-r * T);
}

int main() {
    double S = 100.0;
    double K = 105.0;
    double r = 0.05;
    double T = 1.0;
    double sigma = 0.2;
    long numeroSimulaciones = 10000000;

    srand((unsigned int)time(NULL));
    
    printf("Iniciando Simulacion de Monte Carlo (Version C Serial)...\n");
    printf("------------------------------------------------------\n");
    printf("Numero de simulaciones: %ld\n", numeroSimulaciones);

    clock_t tiempoInicio, tiempoTermino;
    double tiempoConcurrido;
    
    tiempoInicio = clock();
    double precio_opcion = MontecarloCall(numeroSimulaciones, S, K, r, T, sigma);
    tiempoTermino = clock();
    
    tiempoConcurrido = ((double)(tiempoTermino - tiempoInicio)) / CLOCKS_PER_SEC;

    printf("El precio estimado de la opcion Call es: %.5f\n", precio_opcion);
    printf("Tiempo de ejecucion: %.4f segundos\n", tiempoConcurrido);

    return 0;
}