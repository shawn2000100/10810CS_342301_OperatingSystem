#include "syscall.h"

int main(void)
{
	int success= Create("file0.test"); // 191012[J]: 後續可以再拿來測試open, write, read, close
	if (success != 1) MSG("Failed on creating file");
	MSG("Success on creating file0.test");
	Halt();
}

