#include <termios.h>
#include <stdio.h>

int main()
{
	struct termios term;

	tcgetattr(0, &term);
	if (term.c_lflag & ECHO) {
		printf("ECHO in c_lflag is set.\n");
		printf("ECHO = 0x%x\n", ECHO);
		printf("c_lflag = 0x%x\n", term.c_lflag);
		term.c_lflag &= ~ECHO;
		printf("c_lflag = 0x%x\n", term.c_lflag);
		if (tcsetattr(0,TCSANOW, &term)) {
			printf("Fail to unset ECHO.\n");
			return 1;
		}
		if (term.c_lflag & ECHO) {
			printf("ECHO fail to clear.\n");
		}
		printf("ECHO is unset.\n");
		while(1) {
			char c = getchar();
			putchar(c);
		}
	} else
		printf("ECHO c_lflag is not set.\n");

	return 0;
}
