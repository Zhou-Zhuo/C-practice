#include <stdio.h>

int main()
{
	FILE *fp = fopen("./xxx", "w+");

	if (!fp)
		fprintf(stderr, "fail to open file\n");

	while (getchar() != 'x') {
		printf("get\n");
		fprintf(fp, "abcdef\n");
	}
	fclose(fp);

	return 0;
}
