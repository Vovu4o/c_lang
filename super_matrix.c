#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 3  // Размер матриц (например, 3x3)


// Функция для вывода матрицы
void print_matrix(int (*matrix)[SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// Функция для умножения строки матрицы A на матрицу B и записи результата в матрицу C
void multiply_row(int (*A)[SIZE], int (*B)[SIZE], int (*C)[SIZE], int row) {
    for (int j = 0; j < SIZE; j++) {
        C[row][j] = 0;
        for (int k = 0; k < SIZE; k++) {
            C[row][j] += A[row][k] * B[k][j];
        }
    }
}

int main() {
    int (*A)[SIZE], (*B)[SIZE], (*C)[SIZE];  // Указатели на матрицы A, B, C

    // Выделение разделяемой памяти для всех матриц
    A = mmap(NULL, SIZE * SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    B = mmap(NULL, SIZE * SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    C = mmap(NULL, SIZE * SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (A == MAP_FAILED ||  B == MAP_FAILED  || C == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Инициализация матриц A и B
    printf("Введите элементы матрицы A (%dx%d):\n", SIZE, SIZE);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            scanf("%d", &A[i][j]);
        }
    }

    printf("Введите элементы матрицы B (%dx%d):\n", SIZE, SIZE);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            scanf("%d", &B[i][j]);
        }
    }

    // Вывод матриц A и B
    printf("Матрица A:\n");
    print_matrix(A);

    printf("Матрица B:\n");
    print_matrix(B);

    // Создание дочерних процессов для вычисления строк результирующей матрицы C
    for (int i = 0; i < SIZE; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Дочерний процесс вычисляет i-ю строку матрицы C
            multiply_row(A, B, C, i);
            munmap(A, SIZE * SIZE * sizeof(int));
            munmap(B, SIZE * SIZE * sizeof(int));
            munmap(C, SIZE * SIZE * sizeof(int));
            exit(0);  // Завершаем дочерний процесс
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        }
    }

    // Ожидание завершения всех дочерних процессов
    for (int i = 0; i < SIZE; i++) {
        wait(NULL);
    }

    // Вывод результирующей матрицы C
    printf("Результирующая матрица C (A x B):\n");
    print_matrix(C);

    // Освобождение разделяемой памяти
    munmap(A, SIZE * SIZE * sizeof(int));
    munmap(B, SIZE * SIZE * sizeof(int));
    munmap(C, SIZE * SIZE * sizeof(int));

    return 0;
}
