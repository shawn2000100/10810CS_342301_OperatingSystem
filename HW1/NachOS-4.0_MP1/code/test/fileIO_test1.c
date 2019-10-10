#include "syscall.h"

// 1910010[J]: 之後demo要跑的程式，可能要實作出syscall裡的Write, Close功能。下面才能編譯成功
int main(void)
{
	char test[] = "abcdefghijklmnopqrstuvwxyz";
	int success= Create("file1.test");
	OpenFileId fid;
	int i;
	if (success != 1) MSG("Failed on creating file");
	fid = Open("file1.test");
	
	if (fid <= 0) MSG("Failed on opening file");
	
	for (i = 0; i < 26; ++i) {
		int count = Write(test + i, 1, fid);
		if (count != 1) MSG("Failed on writing file");
	}
       
	success = Close(fid);
	if (success != 1) MSG("Failed on closing file");
	MSG("Success on creating file1.test");
	Halt();
}

