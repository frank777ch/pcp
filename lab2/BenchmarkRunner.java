import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.stream.Collectors;

public class BenchmarkRunner {

    public static List<Long> factorize(long number) {
        List<Long> factors = new ArrayList<>();
        long n = number;
        for (long i = 2; i * i <= n; i++) {
            while (n % i == 0) {
                factors.add(i);
                n /= i;
            }
        }
        if (n > 1) {
            factors.add(n);
        }
        return factors;
    }

    public static void main(String[] args) {
        
        int[] inputSizes = {100, 200, 300, 500};
        
        final int MAX_NUMBER = 50_000_000;
        final int MIN_NUMBER = 40_000_000;

        System.out.println("Iniciando Benchmark de Factorización Secuencial vs. Paralelo");
        System.out.println("=====================================================================");
        System.out.printf("%-20s | %-20s | %-20s | %-10s\n", "Tamaño Entrada (N)", "Tiempo Secuencial (s)", "Tiempo Paralelo (s)", "Speedup");
        System.out.println("---------------------------------------------------------------------");

        List<List<Long>> resultsSeq = null;
        List<List<Long>> resultsPar = null;

        for (int size : inputSizes) {
            Random random = new Random();
            List<Long> numbers = new ArrayList<>();
            for (int i = 0; i < size; i++) {
                numbers.add((long) random.nextInt(MAX_NUMBER - MIN_NUMBER) + MIN_NUMBER);
            }

            long startTimeSeq = System.nanoTime();
            resultsSeq = numbers.stream().map(BenchmarkRunner::factorize).collect(Collectors.toList());
            long endTimeSeq = System.nanoTime();
            double durationSeq = (endTimeSeq - startTimeSeq) / 1_000_000_000.0;

            long startTimePar = System.nanoTime();
            resultsPar = numbers.parallelStream().map(BenchmarkRunner::factorize).collect(Collectors.toList());
            long endTimePar = System.nanoTime();
            double durationPar = (endTimePar - startTimePar) / 1_000_000_000.0;

            double speedup = durationSeq / durationPar;
            System.out.printf("%-20d | %-20.4f | %-20.4f | %-10.2fx\n", size, durationSeq, durationPar, speedup);
        }
        
        System.out.println("=====================================================================");
        System.out.println("Benchmark completado.");

        if (resultsSeq != null && resultsPar != null) {
            System.out.println("Verificación final (dummy): Total de resultados secuenciales = " + resultsSeq.size());
        }
    }
}