#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
   int pipefd[2];
   pid_t cpid;
   unsigned char tt;
   char buf[128];
   char rbuf;
   int rr,wt;
   if (argc != 2){
      fprintf(stderr,"Usage %s <string>\n",argv[0]);
      exit(EXIT_FAILURE);
   }
   srand(time(NULL));
   if(pipe(pipefd)==-1){
      perror("pipe");
      exit(EXIT_FAILURE);
   }
	// when we create new process, we should remember about max size of forks; stack size, so we should use alloc and malloc; 
   tt=(unsigned char)rand()%5+2;
   sprintf(buf,"time[%X]\n",tt);
   // waitpid - allow return code of finish
   write(STDOUT_FILENO,buf,strlen(buf));
   if((cpid=fork())==-1){
      perror("pid");
      exit(EXIT_FAILURE);
   }
   // dup2(pipe_fd[1], 1) # write in pipe
   exit(0);
}
