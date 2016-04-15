#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define BUF_LEN 256
char rw_buff[BUF_LEN];

int main(int argc, char **argv)
{
	int count;
	int len;
	int fd;

	if (argc != 2) {
		fprintf(stderr, "Specific a command.\n");
		return -1;
	}

	strncpy(rw_buff, argv[1], BUF_LEN);
	strcat(rw_buff, "\r");
	len = strlen(rw_buff);

	fd = open("/dev/smd0", O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Failed to open smd0.\n");
		return fd;
	}

	count = write(fd, rw_buff, len);
	if (count != len) {
		fprintf(stderr, "Failed to write cmd.\n");
		return count;
	}

	count = read(fd, rw_buff, BUF_LEN - 1);
	if (count < 0) {
		fprintf(stderr, "Failed to read from smd0.\n");
		return count;
	}
	rw_buff[count] = '\0';
	printf("Got %d bytes:\n%s\n", count, rw_buff);

	sleep(1);

	count = read(fd, rw_buff, BUF_LEN - 1);
	if (count < 0) {
		fprintf(stderr, "Failed to read from smd0.\n");
		return count;
	}

	rw_buff[count] = '\0';
	printf("Got %d bytes:\n%s\n", count, rw_buff);
}
