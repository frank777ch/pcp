import numpy as np
import multiprocessing

MAX = 500

a = np.zeros((MAX, MAX), dtype=np.int64)
b = np.zeros((MAX, MAX), dtype=np.int64)

def multiply_row(row_index):
    result_row = np.zeros(MAX, dtype=np.int64)
    for j in range(MAX):
        for k in range(MAX):
            result_row[j] += a[row_index, k] * b[k, j]
    return result_row

def main():
    for i in range(MAX):
        for j in range(MAX):
            b[i, j] = 100 + i
        a[i, i] = 1

    with multiprocessing.Pool() as pool:
        result_rows = pool.map(multiply_row, range(MAX))

    c = np.array(result_rows)

    for i in range(MAX):
        for j in range(MAX):
            print(f"{c[i, j]} ", end="", flush=True)
        print()

if __name__ == '__main__':
    main()