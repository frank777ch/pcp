import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.stream.Collectors;

public class FactorizationParallel {

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
        final int NUMBER_COUNT = 100;
        final int MAX_NUMBER = 500_000;
        Random random = new Random();
        List<Long> numbers = new ArrayList<>();
        for (int i = 0; i < NUMBER_COUNT; i++) {
            numbers.add((long) random.nextInt(MAX_NUMBER - 100_000) + 100_000);
        }
        System.out.println("Generados " + NUMBER_COUNT + " números para factorizar.");
        System.out.println("======================================================");

        System.out.println("Iniciando la factorización PARALELA...");
        long startTime = System.nanoTime();

        List<List<Long>> results = numbers.parallelStream()
                .map(FactorizationParallel::factorize)
                .collect(Collectors.toList());

        long endTime = System.nanoTime();
        double duration = (endTime - startTime) / 1_000_000_000.0;
        
        System.out.printf("La factorización PARALELA tomó: %.4f segundos\n", duration);
        System.out.println("Procesamiento completado.");
    }
}