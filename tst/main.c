#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
	static char filename[] = "/sys/devices/swt-panel.86/enable";
	static char buf[64];
	int fd = 0;
	int i;

	fd = open(filename, O_RDWR);
	if (fd < 0) {
		printf("Fail to open file.\n");
		return -1;
	} else
		printf("Success to open.\n");

	if (read(fd, buf, 64) < 0) {
		printf("Fail to read file.\n");
		return -2;
	}

	for (i = 0; buf[i] && i < 64; i++) {
		printf("0x%x", buf[i]);
	}
	printf("\n");

	return 0;
}
