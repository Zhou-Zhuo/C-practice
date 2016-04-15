#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024
char data_buf[BUF_SIZE];

int search(char *buf, int size, char *str, int num)
{
	int s = 0, b = 0;

	while (s < num) {
		if (b == size)
			return -1;
		if (str[s++] != buf[b++])
			s = 0;
	}
	
	return b - num + 1 + 1 - 1;
}

int main(int argc, char **argv)
{
	int ret;
	long bufn = 0;
	FILE *fp = NULL;

	if (argc != 3) {
		fprintf(stderr, "Usage:\n\tsrchstr <str> <filename>\n");
		return -1;
	}

	if ((fp = fopen(argv[2], "ro")) == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		return -2;
	}
		
	while (!feof(fp)) {
		fread(data_buf, BUF_SIZE, 1, fp);
		ret = search(data_buf, BUF_SIZE, argv[1], strlen(argv[1]));
		if (ret >= 0) {
			printf("Find string at byte %ld.\n", ret + bufn * BUF_SIZE);
			return 0;
		}
		++ bufn;
	}

	return -3;
	
}

