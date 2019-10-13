/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 **************************************************************/

// 1910010[J]: Hint: 這個檔案需要被修改!
// 1910010[J]: syscall似乎是給user program看的介面，而ksyscall則是在kernel mode下真正呼叫system call的功能
// 191012[J]: 可能是軟體工程的考量，ksyscall又會再度呼叫真正底層的功能實作 
// 191012[J]: 以File Operation為例，ksyscall其實只是呼叫了filesys.h裡實作的功能
#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"


void SysHalt()
{
  kernel->interrupt->Halt();
}

void SysPrintInt(int val)
{ 
  DEBUG(dbgTraCode, "In ksyscall.h:SysPrintInt, into synchConsoleOut->PutInt, " << kernel->stats->totalTicks);
  kernel->synchConsoleOut->PutInt(val);
  DEBUG(dbgTraCode, "In ksyscall.h:SysPrintInt, return from synchConsoleOut->PutInt, " << kernel->stats->totalTicks);
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

int SysCreate(char *filename)
{
	// return value
	// 1: success
	// 0: failed
	return kernel->fileSystem->Create(filename);
}


// 1910010[J]: 以下為後來新定義的4個介面
// 1910010[J]: ----------------------------------------------------------
// Open a file with the name, and returns its corresponding OpenFileId. 
// Return -1 if fail to open the file.
OpenFileId SysOpen(char *name) // 191012[J]: 似乎可以了?
{
  return kernel->fileSystem->OpenAFile(name);
}

// Write “size” characters from the buffer into the file, 
// and return the number of characters actually written to the file. 
// Return -1, if fail to write the file. 
int SysWrite(char *buffer, int size, OpenFileId id)
{
  return kernel->fileSystem->WriteFile0(buffer, size, id);
}

// Read “size” characters from the file to the buffer, 
// and return the number of characters actually read from the file. 
// Return -1, if fail to read the file.
int SysRead(char *buffer, int size, OpenFileId id)
{
  return kernel->fileSystem->ReadFile(buffer, size, id);
}

// Close the file with id. 
// Return 1 if successfully close the file. Otherwise, return -1.
int SysClose(OpenFileId id)
{
  return kernel->fileSystem->CloseFile(id);
}
// 1910010[J]: ----------------------------------------------------------

#endif /* ! __USERPROG_KSYSCALL_H__ */
