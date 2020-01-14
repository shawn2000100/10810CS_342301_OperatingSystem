#include "syscall.h"

int
main()
{
	int n;
	for (n=9;n>5;n--) {
		PrintInt(n);
	}
        Halt();
}

