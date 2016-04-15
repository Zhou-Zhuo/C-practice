#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>

#define BUF_LEN 256
char tx_buff[BUF_LEN];
char rx_buff[BUF_LEN];
int fd;

static void *rx_thread(void *arg)
{
	int count;

	while (1) {
		count = read(fd, rx_buff, BUF_LEN - 1);
		if (count < 0) {
			fprintf(stderr, "rx_thread read fault!\n");
			pthread_exit((void *)(&count));
		}
		rx_buff[count] = '\0';
		printf("Got %d bytes:\n%s\n", count, rx_buff);
	}
}

int main(int argc, char **argv)
{
	int count;
	int len;
	pthread_t tid_rx;
	int ret;

	fd = open("/dev/smd0", O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Failed to open smd0.\n");
		return fd;
	}

	ret = pthread_create(&tid_rx, NULL, rx_thread, NULL);
	if (ret) {
		fprintf(stderr, "Failed to create rx_thread.\n");
		return ret;
	}

	while (1) {
		printf("Input an AT cmd:\n");
		fgets(tx_buff, BUF_LEN - 1, stdin);
		strcat(tx_buff, "\r");
		len = strlen(tx_buff);
		count = write(fd, tx_buff, len);
		if (count != len)
			fprintf(stderr, "Failed to write cmd.\n");
	}

}
