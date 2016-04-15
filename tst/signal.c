#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void signal_routine(int signal)
{
	switch (signal) {
		case SIGINT:
			printf("int.\n");
			exit(0);
		case SIGKILL:
			printf("kill.\n");
			exit(0);
		case SIGTERM:
			printf("term.\n");
			exit(0);
	}
}

int main()
{
	signal()
}
