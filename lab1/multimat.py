import numpy as np

MAX=500
a = np.arange(MAX * MAX).reshape(MAX,MAX)
b = np.arange(MAX * MAX).reshape(MAX,MAX)
c = np.arange(MAX * MAX).reshape(MAX,MAX)

for i in range(0,MAX):
    for j in range(0,MAX):
        a[i][j] = 0
        b[i][j] = 100 + i
    a[i][i] = 1

for i in range(0,MAX):
    for j in range(0,MAX):
        c[i][j] = 0
        for k in range(0,MAX):
            c[i][j] = c[i][j] + a[i][k] * b[k][j]

for i in range(0,MAX):
    for j in range(0,MAX):
        print(f"{c[i][j]}  ",end="")
    print()