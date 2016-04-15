#include <stdio.h>

extern char **environ;

int main()
{
	int i;

	for (i = 0; environ[i] != NULL; i++)
		printf("%s\n", environ[i]);

	//printf("\n");

	return 0;
}
