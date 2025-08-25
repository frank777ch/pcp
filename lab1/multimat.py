import numpy as np
#
# Creación de variables
#
MAX=500
a = np.arange(MAX * MAX).reshape(MAX,MAX)
b = np.arange(MAX * MAX).reshape(MAX,MAX)
c = np.arange(MAX * MAX).reshape(MAX,MAX)

#
# Inicialización de las matrices. Observe que A será la matriz identidad
#
for i in range(0,MAX):
    for j in range(0,MAX):
        a[i][j] = 0
        b[i][j] = 100 + i
    a[i][i] = 1

#
# Multiplicación efectivamente de las matrices
#
for i in range(0,MAX):
    for j in range(0,MAX):
        c[i][j] = 0
        for k in range(0,MAX):
            c[i][j] = c[i][j] + a[i][k] * b[k][j]

#
# Impresión de la matriz (C) resultado de multiplicar A x B
#
for i in range(0,MAX):
    for j in range(0,MAX):
        print(f"{c[i][j]}  ",end="")
    print()