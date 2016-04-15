#include <stdio.h>
#include <stdlib.h>

int main()
{
	if (setuid(0))
		fprintf(stderr, "setuid failed.\n");
	system("sh");
}
