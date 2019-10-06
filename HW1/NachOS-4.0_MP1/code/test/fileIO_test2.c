#include "syscall.h"

int main(void)
{
	// you should run fileIO_test1 first before running this one
	char test[26];
	char check[] = "abcdefghijklmnopqrstuvwxyz";
	OpenFileId fid;
	int count, success, i;
	fid = Open("file1.test");
	if (fid <= 0) MSG("Failed on opening file");
	count = Read(test, 26, fid);
	if (count != 26) MSG("Failed on reading file");
	success = Close(fid);
	if (success != 1) MSG("Failed on closing file");
	for (i = 0; i < 26; ++i) {
		if (test[i] != check[i]) MSG("Failed: reading wrong result");
	}
	MSG("Passed! ^_^");
	Halt();
}

