#include <stdio.h>

#define MAX 500

int main()
{

  int a[MAX][MAX],
      b[MAX][MAX],
      c[MAX][MAX];
  int i, j, k;

  for (i = 0; i < MAX; i++)
  {
    for (j = 0; j < MAX; j++)
    {
      a[i][j] = 0;
      b[i][j] = 100 + i;
    }
    a[i][i] = 1;
  }

  for (i = 0; i < MAX; i++)
  {
    for (j = 0; j < MAX; j++)
    {
      c[i][j] = 0;
      for (k = 0; k < MAX; k++)
      {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }

  for (i = 0; i < MAX; i++)
  {
    for (j = 0; j < MAX; j++)
      printf("%5d ", c[i][j]);
    printf("\n");
  }
  return 0;
}