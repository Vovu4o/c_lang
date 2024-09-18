#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 3  

void print_matrix(int (*matrix)[SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}


void multiply_row(int (*A)[SIZE], int (*B)[SIZE], int (*C)[SIZE], int row) {
    for (int j = 0; j < SIZE; j++) {
        C[row][j] = 0;
        for (int k = 0; k < SIZE; k++) {
            C[row][j] += A[row][k] * B[k][j];
        }
    }
}

int main() {
    int (*A)[SIZE], (*B)[SIZE], (*C)[SIZE];  
    
    A = mmap(NULL, SIZE * SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    B = mmap(NULL, SIZE * SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    C = mmap(NULL, SIZE * SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (A == MAP_FAILED ||  B == MAP_FAILED  || C == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    
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

    printf("Матрица A:\n");
    print_matrix(A);

    printf("Матрица B:\n");
    print_matrix(B);

    for (int i = 0; i < SIZE; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            multiply_row(A, B, C, i);
            munmap(A, SIZE * SIZE * sizeof(int));
            munmap(B, SIZE * SIZE * sizeof(int));
            munmap(C, SIZE * SIZE * sizeof(int));
            exit(0);  
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        }
    }

    for (int i = 0; i < SIZE; i++) {
        wait(NULL);
    }

    printf("Результирующая матрица C (A x B):\n");
    print_matrix(C);

    munmap(A, SIZE * SIZE * sizeof(int));
    munmap(B, SIZE * SIZE * sizeof(int));
    munmap(C, SIZE * SIZE * sizeof(int));

    return 0;
}