#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FILENAME "./test"

int main()
{
	int fd;

	fd = open(FILENAME, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "error on opening %s.\n", FILENAME);
		return 1;
	}

	getchar();

	return 0;
}
