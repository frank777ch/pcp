#include <iostream>     // Para poder mostrar resultados en la consola (std::cout)
#include <cmath>        // Para las funciones matemáticas: exp(), sqrt(), log(), max()
#include <random>       // Para la generación de números aleatorios de alta calidad (el motor de 'Z')
#include <iomanip>      // Para formatear la salida y que se vea más limpia (std::setprecision)
#include <chrono>       // Para medir el tiempo de ejecución

// Función para valorar la opción call europea mediante Montecarlo
double montecarlo_call_price(
    const long num_simulations, // Número de simulaciones (el 'N' grande)
    const double S,             // Precio inicial de la acción (S_0)
    const double K,             // Precio de ejercicio (Strike price)
    const double r,             // Tasa de interés libre de riesgo
    const double T,             // Tiempo hasta el vencimiento (en años)
    const double sigma)         // Volatilidad de la acción
{
    // --- PASO 1: Configurar el generador de números aleatorios ---
    // Motor generador de números aleatorios de alta calidad (Mersenne Twister)
    std::mt19937 generador(std::random_device{}());
    // Distribución: Queremos una normal estándar (media 0, desviación estándar 1) para generar 'Z'
    std::normal_distribution<> distribucion_normal(0.0, 1.0);

    // Variable para acumular la suma de los beneficios (payoffs)
    double suma_payoffs = 0.0;

    // --- PASO 2: El Bucle Principal de la Simulación ---
    // Se repite el "experimento" el número de veces que especificamos
    for (long i = 0; i < num_simulations; ++i) {
        
        // a. Genera un número aleatorio 'Z' de la distribución normal estándar
        double Z = distribucion_normal(generador);

        // b. Calcula UN posible precio final de la acción (S_T) usando la fórmula del GBM
        // S_T = S_0 * exp( (r - 0.5*sigma^2)*T + sigma*sqrt(T)*Z )
        double S_T = S * std::exp((r - 0.5 * sigma * sigma) * T + sigma * std::sqrt(T) * Z);

        // c. Calcula el beneficio (payoff) para una opción Call en este futuro simulado
        // payoff = max(S_T - K, 0)
        double payoff = std::max(0.0, S_T - K);

        // d. Acumula el payoff a la suma total
        suma_payoffs += payoff;
    }

    // --- PASO 3: Calcular el Precio Final de la Opción ---
    // e. Calcula el promedio de los payoffs
    double promedio_payoff = suma_payoffs / num_simulations;

    // f. Descuenta el promedio a valor presente para obtener el precio de la opción hoy
    // precio = promedio_payoff * exp(-r*T)
    return promedio_payoff * std::exp(-r * T);
}


int main() {
    // --- Parámetros del problema financiero ---
    double S = 100.0;      // Precio inicial de la acción
    double K = 105.0;      // Precio de ejercicio (Strike)
    double r = 0.05;       // Tasa de interés libre de riesgo (5%)
    double T = 1.0;        // Tiempo hasta el vencimiento (1 año)
    double sigma = 0.2;    // Volatilidad de la acción (20%)
    
    // --- Parámetros de la simulación ---
    long num_simulations = 1000000; // Número de simulaciones (1 millón)

    std::cout << "Iniciando Simulación de Monte Carlo (Versión Serial)..." << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;
    std::cout << "Número de simulaciones: " << num_simulations << std::endl;

    // Medimos el tiempo de inicio
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Llamamos a la función que hace el cálculo
    double precio_opcion = montecarlo_call_price(num_simulations, S, K, r, T, sigma);
    
    // Medimos el tiempo de finalización
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    // --- Mostrar el resultado ---
    std::cout << std::fixed << std::setprecision(5); // Formatear para ver 5 decimales
    std::cout << "El precio estimado de la opción Call es: " << precio_opcion << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed_time.count() << " segundos" << std::endl;

    return 0;
}