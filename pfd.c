#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>



int child_pipe_call(unsigned char *tt, char *buf[], int *pipefd[], char *argv[]) {
	 close(pipefd[0]);
     dup2(pipefd[1],1);
     close(pipefd[1]);
     write(STDOUT_FILENO, argv[1], strlen(argv[1]));
     sleep(tt);
     sprintf(buf,"exit %d\n",getpid());
     write(STDOUT_FILENO,buf,strlen(buf));
     _exit(EXIT_SUCCESS);
}
int main(int argc, char *argv[]) {
  int pipefd[2];
  pid_t cpid;
  unsigned char tt;
  char buf[128];
  char rbuf;
  int rr,wt;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <string>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  for (int i=0; i<3; i++) {
  tt=(unsigned char)rand()%5+2;
  sprintf(buf,"time[%X]\n",tt);
  write(STDOUT_FILENO,buf,strlen(buf));
   
  if ((cpid = fork()) == -1) {
     perror("fork");
     exit(EXIT_FAILURE);
  }

  if (cpid == 0) {    
     /*close(pipefd[0]);
     dup2(pipefd[1],1);
     close(pipefd[1]);
     write(STDOUT_FILENO, argv[1], strlen(argv[1]));
     sleep(tt);
     sprintf(buf,"exit %d\n",getpid());
     write(STDOUT_FILENO,buf,strlen(buf));
     _exit(EXIT_SUCCESS); */
  } else {           
     close(pipefd[1]);
  }

  printf("waiting...\n");
  while ((wt=waitpid(-1,NULL,WNOHANG)) > -1){
      if (cpid == wt) {
        while ((rr=read(pipefd[0], &rbuf, 1)) > 0){
           write(STDOUT_FILENO, &rbuf, 1);
           sprintf(buf,"[%X]",rbuf);
           write(STDOUT_FILENO,buf,strlen(buf));
        }   
        if(rr == 0 ) {
           write(STDOUT_FILENO, "\n", 1);
           close(pipefd[0]);    
        }
      }
    usleep(10000); 
  }
  if (errno == ECHILD) 
     perror("finish");
  else
     perror("wait error");
  }
  exit(EXIT_SUCCESS);
}

