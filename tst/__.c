#include <stdio.h>

#define _ re
#define ___ tu
#define _____ rn
#define ______(_,__) __##_
#define ____(_,__) _##__

int main()
{
	int __ = 5;
	printf("%d\n", __);

	______(____(tu,rn),re) __-__;
}
