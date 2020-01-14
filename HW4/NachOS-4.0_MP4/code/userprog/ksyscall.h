/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"

#include "synchconsole.h"


void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

#ifdef FILESYS_STUB
int SysCreate(char *filename)
{
	// return value
	// 1: success
	// 0: failed
	return kernel->interrupt->CreateFile(filename);
}
#else
// 200112[J]: MP4·s¼W
int SysCreate(char *filename, int size)
{
	return kernel->fileSystem->CreateFile0(filename, size);
}

OpenFileId SysOpen(char *name)
{
  return kernel->fileSystem->OpenFile0(name);
}

int SysWrite(char *buffer, int size, OpenFileId id)
{
  return kernel->fileSystem->WriteFile0(buffer, size, id);
}

int SysRead(char *buffer, int size, OpenFileId id)
{
  return kernel->fileSystem->ReadFile0(buffer, size, id);
}

int SysClose(OpenFileId id)
{
  return kernel->fileSystem->CloseFile0(id);
}
// **********************************************************************
#endif


#endif /* ! __USERPROG_KSYSCALL_H__ */
