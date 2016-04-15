#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define GSERIAL_IOCTL_MAGIC		'G'
#define GSERIAL_SMD_WRITE		_IOW(GSERIAL_IOCTL_MAGIC, 1, \
					struct ioctl_smd_rw_arg_type)
#define GSERIAL_SMD_READ		_IOR(GSERIAL_IOCTL_MAGIC, 0, \
					struct ioctl_smd_rw_arg_type)
#define GSERIAL_SET_XPORT_TYPE		_IOW(GSERIAL_IOCTL_MAGIC, 0, unsigned int)
#define GSERIAL_SMD_DATA_DISCARD        _IO(GSERIAL_IOCTL_MAGIC, 0)

#define GSERIAL_BUF_LEN		256

#define GSERIAL_SET_XPORT_TYPE_TTY 0
#define GSERIAL_SET_XPORT_TYPE_SMD 1

struct ioctl_smd_rw_arg_type {
	char			*buf;
	volatile unsigned	size;
};

static void rx_thread(void *arg)
{
	struct ioctl_smd_rw_arg_type smd_rw_arg;
	static char smd_rw_buff[GSERIAL_BUF_LEN];

	while(1) {
		smd_rw_arg.size = GSERIAL_BUF_LEN - 1;
		ret = ioctl(fd, GSERIAL_SMD_READ, &smd_rw_arg);
		smd_rw_buff[smd_rw_arg.size] = '\0';
		if (ret < 0) {
			fprintf(stderr, "Failed to get echo from android_serial_device.\n");
			exit(ret);
		}

		printf("Echo from android_serial_device:\n%s\n", smd_rw_buff);
	}
}

static void tx_thread(void *arg)
{
	struct ioctl_smd_rw_arg_type smd_rw_arg;
	static char smd_rw_buff[GSERIAL_BUF_LEN];
	
}

int main(int argc, char **argv)
{
	struct ioctl_smd_rw_arg_type smd_rw_arg;
	static char smd_rw_buff[GSERIAL_BUF_LEN];
	int ret;
	int fd;
	int tid_tx, tid_rx;
	int xport_type;

	if (argc != 2) {
		fprintf(stderr, "Specific a command.\n");
		return -1;
	}

	strncpy(smd_rw_buff, argv[1], GSERIAL_BUF_LEN);
	strcat(smd_rw_buff, "\r");

	fd = open("/dev/android_serial_device", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to open android_serial_device.\n");
		return fd;
	}

	if (ret = pthread_create(&tid_rx, NULL, tx_thread, NULL)) {
		fprintf(stderr, "Failed to create tx_thread.\n");
		return ret;
	}

	if (ret = pthread_create(&tid_rx, NULL, rx_thread, NULL)) {
		fprintf(stderr, "Failed to create rx_thread.\n");
		return ret;
	}

	xport_type = GSERIAL_SET_XPORT_TYPE_SMD;
	ret = ioctl(fd, GSERIAL_SET_XPORT_TYPE, &xport_type);
	if (ret < 0) {
		fprintf(stderr, "Failed set gserial xport type.\n");
		return ret;
	}

	smd_rw_arg.buf = smd_rw_buff;
	smd_rw_arg.size = strlen(smd_rw_buff);
	
	ret = ioctl(fd, GSERIAL_SMD_WRITE, &smd_rw_arg);
	if (ret < 0) {
		fprintf(stderr, "Failed to put cmd to android_serial_device.\n");
		return ret;
	}

	return 0;
}
