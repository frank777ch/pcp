import java.util.stream.IntStream;

public class MultiMatParallel {
    private static final int MAX = 500;

    public static void main(String[] args) {
        int[][] a = new int[MAX][MAX];
        int[][] b = new int[MAX][MAX];
        int[][] c = new int[MAX][MAX];
        
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                a[i][j] = 0;
                b[i][j] = 100 + i;
            }
            a[i][i] = 1;
        }

        IntStream.range(0, MAX)
                 .parallel()
                 .forEach(i -> {
                     for (int j = 0; j < MAX; j++) {
                         c[i][j] = 0; 
                         for (int k = 0; k < MAX; k++) {
                             c[i][j] += a[i][k] * b[k][j];
                         }
                     }
                 });

        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                System.out.print(c[i][j] + " ");
            }
            System.out.println();
        }
    }
}