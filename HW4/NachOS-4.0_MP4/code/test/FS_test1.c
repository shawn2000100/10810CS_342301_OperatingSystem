#include "syscall.h"

int main(void)
{
	char test[] = "abcdefghijklmnopqrstuvwxyz\n";
	int success = Create("/file1", 27);
	OpenFileId fid;
	int i;
	if (success != 1) MSG("Failed on creating file");
	fid = Open("/file1");
	if (fid < 0) MSG("Failed on opening file");
	for (i = 0; i < 27; ++i) {
		int count = Write(test + i, 1, fid);
		if (count != 1) MSG("Failed on writing file");
	}
	success = Close(fid);
	if (success != 1) MSG("Failed on closing file");
	Halt();
}

