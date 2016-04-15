#include <stdio.h>

int main()
{
	char str[] = "fadsbx";
	char buf[64];

	if (sscanf(str, "adsb%s", buf))
		printf("true\n");
	else
		printf("false\n");

	return 0;
}
