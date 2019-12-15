#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

extern void Abort();
extern void Exit(int exitCode);
extern void Delay(int seconds);
extern void UDelay(unsigned int usec);

extern void CallOnUserAbort(void (*cleanup)(int));

extern void RandomInit(unsigned seed);
extern unsigned int RandomNumber();

extern char *AllocBoundedArray(int size);
extern void DeallocBoundedArray(char *p, int size);

extern bool PollFile(int fd);

extern int OpenForWrite(char *name);
extern int OpenForReadWrite(char *name, bool crashOnError);
extern void Read(int fd, char *buffer, int nBytes);
extern int ReadPartial(int fd, char *buffer, int nBytes);
extern void WriteFile(int fd, char *buffer, int nBytes);
extern void Lseek(int fd, int offset, int whence);
extern int Tell(int fd);
extern int Close(int fd);
extern bool Unlink(char *name);


extern "C" {
    int atoi(const char *str);
    double atof(const char *str);
    int abs(int i);
    void bcopy(const void *s1, void *s2, size_t n);
    void bzero(void *s, size_t n);
}


extern int OpenSocket();
extern void CloseSocket(int sockID);
extern void AssignNameToSocket(char *socketName, int sockID);
extern void DeAssignNameToSocket(char *socketName);
extern bool PollSocket(int sockID);
extern void ReadFromSocket(int sockID, char *buffer, int packetSize);
extern void SendToSocket(int sockID, char *buffer, int packetSize,char *toName);
