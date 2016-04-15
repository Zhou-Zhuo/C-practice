#include <stdio.h>
#include <string.h>

int (*debug)(const char *fmt, ...);

int debug_info(const char *fmt, ...)
{
}

int main(int argc, char **argv)
{
	if (argc > 1 && !strcmp(argv[1], "debug"))
		debug = printf;
	else
		debug = debug_info;

	debug("%d\n", 3);
	debug("%d\n", 4);

	return 0;
}
