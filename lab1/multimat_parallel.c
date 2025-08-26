#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX 500

int main() {
  int (*a)[MAX] = malloc(sizeof(int[MAX][MAX]));
  int (*b)[MAX] = malloc(sizeof(int[MAX][MAX]));
  int (*c)[MAX] = malloc(sizeof(int[MAX][MAX]));

  if (a == NULL || b == NULL || c == NULL) {
      printf("Error: No se pudo asignar memoria.\n");
      return 1;
  }
  
  int i,j,k,tid,nthreads;
  
#pragma omp parallel shared(a,b,c) private(i,j,k)
{
  #pragma omp for
  for (i = 0; i < MAX; i++){
    for (j = 0; j < MAX; j++) {
         a[i][j] = 0;
         b[i][j] = 100 + i;
    }
    a[i][i] = 1;
  }

  #pragma omp for
  for (i = 0; i < MAX; i++){
    for (j = 0; j < MAX; j++) {
       c[i][j] = 0;
       for (k = 0; k < MAX; k++){
         c[i][j] += a[i][k] * b[k][j];
       }
    }
  }
}

  for (i = 0; i < MAX; i++) {
    for (j = 0; j < MAX; j++)
      printf("%5d ", c[i][j]);
    printf("\n");
  }
  
  free(a);
  free(b);
  free(c);

  return 0;
}