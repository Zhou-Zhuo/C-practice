#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int fd;
	char *pbuf;

	fd = open("./main.c", O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Fail to open.\n");
		return -1;
	}

	pbuf = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (pbuf == MAP_FAILED) {
		fprintf(stderr, "Fail to mmap.\n");
		return -2;
	}

	pbuf[2] = 't';
	printf("%s", pbuf);
	close(fd);

	return 0;
}
