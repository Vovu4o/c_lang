#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int fd_src, fd_dst;
	void *addr_src, *addr_dst;
	struct stat filestat;
	if ((fd_src=open(argv[1], O_RDONLY)) < 0 ) 
		{ perror("open 1"); _exit(1); }
	
	if ((fd_dst=open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 00664)) < 0 ) 
		{ perror("open 2"); _exit(1); }
	fstat(fd_src, &filestat);
	lseek(fd_dst, filestat.st_size - 1, SEEK_SET);
	write(fd_dst, "", 1);
	addr_src = mmap(NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd_src, 0);
	addr_dst = mmap(NULL, filestat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dst, 0);
	memcpy(addr_dst, addr_src, filestat.st_size);
	_exit(0);
		
}

