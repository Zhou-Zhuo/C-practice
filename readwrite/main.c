#include "unistd.h"
#include <stdio.h>

#define BUFSIZE 8

int main()
{
	int n;
	char buf[BUFSIZE];

	while ((n = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			fprintf(stderr, "write error\n");

	return 1;

}
