#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#define ROWS 10
#define COLS 10

void fill_mtrx(int matrix[ROWS][COLS])
{
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

int main()
{
    int (*mtrx1)[COLS];
    int (*mtrx2)[COLS];
    int (*res)[COLS];
    int fd1 = open("mtrx1.bin", O_RDWR | O_CREAT | O_TRUNC,00664);
    int fd2 = open("mtrx2.bin", O_RDWR | O_CREAT | O_TRUNC,00664);
    int fd3 = open("res.bin", O_RDWR | O_CREAT | O_TRUNC,00664);
    if(fd1<0 || fd2<0 || fd3<0) {perror("open"); return -1;}
    ftruncate(fd1, sizeof(int)*ROWS*COLS);
    ftruncate(fd2, sizeof(int)*ROWS*COLS);
    ftruncate(fd3, sizeof(int)*ROWS*COLS);
    mtrx1 = mmap(NULL,sizeof(int)*ROWS*COLS, PROT_READ | PROT_WRITE,MAP_SHARED,fd1,0);
    mtrx2 = mmap(NULL,sizeof(int)*ROWS*COLS, PROT_READ | PROT_WRITE,MAP_SHARED,fd2,0);
    res = mmap(NULL,sizeof(int)*ROWS*COLS, PROT_READ | PROT_WRITE,MAP_SHARED,fd3,0);
    if(mtrx1<0 || mtrx2<0 || res<0){perror("mmap"); return -1;}
    srand(time(NULL));
    fill_mtrx(mtrx1);
    fill_mtrx(mtrx2);

    int num_proc = ROWS;
    pid_t cpid;
    for (int i = 0; i < num_proc; i++) {
        cpid = fork();
        if (cpid == 0) {
            for (int j = 0; j < COLS; j++) {
                res[i][j]=0;
                for (int k = 0; k < COLS; k++) {
                    res[i][j]+=mtrx1[i][k] * mtrx2[k][j];
                }
            }
            exit(EXIT_SUCCESS);
        } else if(cpid==-1) {perror("fork"); exit(EXIT_FAILURE);}
    }

    for (int i = 0; i < num_proc; i++) {
        wait(NULL);
    }
    if(errno==0) printf("The operation was successful\n");
    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}
