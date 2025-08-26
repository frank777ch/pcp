public class MultiMat {
    private static final int MAX = 500;

    public static void main(String[] args) {
        int[][] a = new int[MAX][MAX];
        int[][] b = new int[MAX][MAX];
        int[][] c = new int[MAX][MAX];
        
        // Inicialización de las matrices a y b. a será la matriz identidad.
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                a[i][j] = 0;
                b[i][j] = 100 + i;
            }
            a[i][i] = 1;
        }

        // Multiplicación de las matrices
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                c[i][j] = 0;
                for (int k = 0; k < MAX; k++) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }

        // Impresión de la matriz C
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                System.out.print(c[i][j] + " ");
            }
            System.out.println();
        }
    }
}