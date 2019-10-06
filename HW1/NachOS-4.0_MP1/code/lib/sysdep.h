// sysdep.h 
//	System-dependent interface.  Nachos uses the routines defined
//	here, rather than directly calling the UNIX library functions, to
//	simplify porting between versions of UNIX, and even to
//	other systems, such as MSDOS and the Macintosh.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSDEP_H
#define SYSDEP_H

#include "copyright.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

// Process control: abort, exit, and sleep
extern void Abort();
extern void Exit(int exitCode);
extern void Delay(int seconds);
extern void UDelay(unsigned int usec);// rcgood - to avoid spinners.

// Initialize system so that cleanUp routine is called when user hits ctl-C
extern void CallOnUserAbort(void (*cleanup)(int));

// Initialize the pseudo random number generator
extern void RandomInit(unsigned seed);
extern unsigned int RandomNumber();

// Allocate, de-allocate an array, such that de-referencing
// just beyond either end of the array will cause an error
extern char *AllocBoundedArray(int size);
extern void DeallocBoundedArray(char *p, int size);

// Check file to see if there are any characters to be read.
// If no characters in the file, return without waiting.
extern bool PollFile(int fd);

// File operations: open/read/write/lseek/close, and check for error
// For simulating the disk and the console devices.
extern int OpenForWrite(char *name);
extern int OpenForReadWrite(char *name, bool crashOnError);
extern void Read(int fd, char *buffer, int nBytes);
extern int ReadPartial(int fd, char *buffer, int nBytes);
extern void WriteFile(int fd, char *buffer, int nBytes);
extern void Lseek(int fd, int offset, int whence);
extern int Tell(int fd);
extern int Close(int fd);
extern bool Unlink(char *name);

// Other C library routines that are used by Nachos.
// These are assumed to be portable, so we don't include a wrapper.
extern "C" {
int atoi(const char *str);
double atof(const char *str);
int abs(int i);
void bcopy(const void *s1, void *s2, size_t n);
void bzero(void *s, size_t n);
}

// Interprocess communication operations, for simulating the network
extern int OpenSocket();
extern void CloseSocket(int sockID);
extern void AssignNameToSocket(char *socketName, int sockID);
extern void DeAssignNameToSocket(char *socketName);
extern bool PollSocket(int sockID);
extern void ReadFromSocket(int sockID, char *buffer, int packetSize);
extern void SendToSocket(int sockID, char *buffer, int packetSize,char *toName);

#endif // SYSDEP_H
