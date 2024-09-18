#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  
#include <libgen.h>
#include <signal.h>
#define FNAME "/tmp/fifo001"
#define STR "Hello, World\n"
#define QUIT "Quit"

int main_r(void);
int main_w(const char *s);

void fifo_del(void){
  unlink(FNAME);
}

int main(int argc, char *argv[]){
  char *name;

  name=basename(argv[0]);  
  printf("%s\n",name);	
  if (strcmp(name,"rfifo") == 0) main_r();
  else
  if (strcmp(name,"wfifo") == 0) {
    if (argc > 1) main_w(argv[1]); else main_w(STR);
  }
  return 0;
}

int main_r(){
  int fd;
  int rd;
  char buf[80];

  if(atexit(fifo_del) == -1) {perror("atexit"); _exit(EXIT_FAILURE);}

  fifo_del();
  if((mknod(FNAME, S_IFIFO|00644,0)) == -1)  {
    perror("fifo");
    exit(0);
  }

  if((fd=open(FNAME, O_RDWR)) == - 1)  {
    perror("fifo");
    exit(0);
  }
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

  bzero(buf,80);
  while (1){
     rd=read(fd, &buf, sizeof(buf));
     if(rd == -1) {
        perror("fifo");
	if (errno == EAGAIN) {
	    sleep(1);
	    continue;
	}else{
	   exit(EXIT_FAILURE);	
	}
     }	     
     dprintf(1,"Data[%d]: ",rd);
     write(1,buf,rd);
     if (!strcmp(buf,QUIT)) exit(0);
     bzero(buf,80);
     sleep(1);
  }
  return 0;
} 

int main_w(const char *s){
  int fd;
  if((fd=open(FNAME, O_RDWR)) == - 1){
    perror("fifo");
    exit(0);
  }
  write(fd,s,strlen(s));
  exit(0);
} 

