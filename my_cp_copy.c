#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
   void *src;
   int fd;
   pid_t cpid;
   struct stat qwerty;
   fd=open(argv[1],O_RDWR,00664);
   fstat(fd,&qwerty);
   src=mmap(NULL,qwerty.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
   printf("src = %p\n",src);
   if((cpid=fork())<0) {perror("fork");_exit(1);}
   if(cpid==0){
      void *qq = src;
      printf("src = %p\n",src);
      printf("qq = %p\n",qq);
      unsigned char c=0;
      int i=0;
      while (i++ < qwerty.st_size) *((char*)src++) = c++;
      printf("src = %p\n",src);
      printf("qq = %p\n",qq);
      msync(qq,MS_SYNC,0);
      _exit(0);
   }
   wait(NULL);
   _exit(0);

}
