#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define FILENAME "./test"

int main()
{
	int fd;
	struct stat st;

	fd = open(FILENAME, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "error opening %s.\n", FILENAME);
		return 1;
	}

	if (fstat(fd, NULL) < 0) {
		//fprintf(stderr, "error stat'ing %s.\n", FILENAME);
		printf("errno = %d\n", errno);
		perror("");
		return 2;
	}

	printf("size of %s: %d\n", FILENAME, (int)(st.st_size));

	return 0;
}

