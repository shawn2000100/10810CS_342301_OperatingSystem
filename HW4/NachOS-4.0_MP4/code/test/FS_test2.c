#include "syscall.h"

int main(void)
{
	// you should run FS_test1 first before running this one
	char test[26];
	char check[] = "abcdefghijklmnopqrstuvwxyz\n";
	OpenFileId fid;
	int count, success, i;
	fid = Open("/file1");
	if (fid < 0) MSG("Failed on opening file");
	count = Read(test, 27, fid);
	if (count != 27) MSG("Failed on reading file");
	success = Close(fid);
	if (success != 1) MSG("Failed on closing file");
	for (i = 0; i < 27; ++i) {
		if (test[i] != check[i]) MSG("Failed: reading wrong result");
	}
	MSG("Passed! ^_^");
	Halt();
}

