import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.stream.Collectors;

public class FactorizationLab {

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

        final int NUMBER_COUNT = 100000;
        final int MAX_NUMBER = 500_000;
        Random random = new Random();
        List<Long> numbers = new ArrayList<>();

        for (int i = 0; i < NUMBER_COUNT; i++) {
            numbers.add((long) random.nextInt(MAX_NUMBER) + 100_000);
        }
        
        System.out.println("Generados " + NUMBER_COUNT + " números para factorizar.");
        System.out.println("======================================================");

        System.out.println("Iniciando la factorización SECUENCIAL...");
        long startTimeSequential = System.nanoTime();

        List<List<Long>> resultsSequential = numbers.stream()
                .map(FactorizationLab::factorize)
                .collect(Collectors.toList());

        long endTimeSequential = System.nanoTime();
        double durationSequential = (endTimeSequential - startTimeSequential) / 1_000_000_000.0;
        System.out.printf("La factorización SECUENCIAL tomó: %.4f segundos\n", durationSequential);
        System.out.println("------------------------------------------------------");


        System.out.println("Iniciando la factorización PARALELA...");
        long startTimeParallel = System.nanoTime();

        List<List<Long>> resultsParallel = numbers.parallelStream()
                .map(FactorizationLab::factorize)
                .collect(Collectors.toList());

        long endTimeParallel = System.nanoTime();
        double durationParallel = (endTimeParallel - startTimeParallel) / 1_000_000_000.0;
        System.out.printf("La factorización PARALELA tomó:   %.4f segundos\n", durationParallel);
        System.out.println("======================================================");
        
        System.out.printf("Mejora (Speedup): %.2f veces más rápido\n", durationSequential / durationParallel);
    }
}