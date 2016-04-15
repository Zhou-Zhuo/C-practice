#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//#define BATTERY_FILE "/sys/class/power_supply/battery/capacity"
#define BATTERY_FILE "batt"

FILE *fp;
int bfd;
int period;

int get_battery(int fd)
{
	int battery = -1;
	char rbuf[64];

	if (lseek(fd, 0, SEEK_SET) < 0)
		return -1;

	if (read(fd, rbuf, 64) <= 0)
		return -1;

	if (!sscanf(rbuf, "%d", &battery))
		return -1;

	return battery;
}

void signal_routine(int signal)
{
	static int num = 0;
	static int now = 0;
	int battery;

	switch (signal) {
		case SIGINT:
		case SIGKILL:
		case SIGTERM:
			fprintf(fp, "==================================\n");
			close(bfd);
			fclose(fp);
			printf("exit.\n");
			exit(0);
		case SIGHUP:
			return;
		case SIGALRM:
			battery = get_battery(bfd);
			if (battery < 0) {
				fprintf(stderr, "error reading battery.\n");
				close(bfd);
				fclose(fp);
				exit(-1);
			}

			fprintf(stdout, "%d\t%d\t%d\n", num, now, battery);
			//fflush(fp);
			alarm(period);
			num ++;
			now += period;
			return;
	}
}

void usage()
{
	fprintf(stderr, "usage: batt_log <secends> <logfilename>\n");
}

int open_battery()
{
	return open(BATTERY_FILE, O_RDONLY);
}

int main(int argc, char **argv)
{
	time_t start;

	if (argc != 3) {
		usage();
		return 1;
	}

	if (!sscanf(argv[1], "%d", &period)) {
		usage();
		return 2;
	}

	fp = fopen(argv[2], "a");
	if (!fp) {
		fprintf(stderr, "error opening logfile %s\n", argv[2]);
		return 3;
	}

	bfd = open_battery();
	if (bfd < 0) {
		fprintf(stderr, "error opening battery %s\n", BATTERY_FILE);
		goto out;
	}

	time(&start);
	fprintf(fp, "%s==================================\n", ctime(&start));
	fprintf(fp, "NUM\tTIME\tBATTERY\n");
	fflush(fp);

	signal(SIGINT, signal_routine);
	signal(SIGKILL, signal_routine);
	signal(SIGTERM, signal_routine);
	signal(SIGHUP, signal_routine);
	signal(SIGALRM, signal_routine);

	signal_routine(SIGALRM);

	while (1)
		pause();

out:
	fclose(fp);
	return 4;
}
