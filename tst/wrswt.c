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

	fd = open(filename, O_RDWR);
	if (fd < 0) {
		printf("Fail to open file.\n");
		return -1;
	} else
		printf("Success to open.\n");

	buf[0] = 1;
	if (write(fd, buf, 1) < 0) {
		printf("Fail to write file.\n");
		return -2;
	}

	return 0;
}
