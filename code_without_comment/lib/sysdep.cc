#include "debug.h"
#include "sysdep.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cerrno>

extern "C" {
#include <signal.h>
#include <sys/types.h>

#ifndef NO_MPROT
#include <sys/mman.h>
#endif

// UNIX routines called by procedures in this file

#if defined CYGWIN
  size_t getpagesize(void);
#else
 int getpagesize(void);
#endif
unsigned sleep(unsigned);
//#ifdef SOLARIS
//int usleep(useconds_t);
//#else
//void usleep(unsigned int);  // rcgood - to avoid spinning processes.
//#endif


#ifndef NO_MPROT

#ifdef OSF
#define OSF_OR_AIX
#endif
#ifdef AIX
#define OSF_OR_AIX
#endif

#ifdef OSF_OR_AIX
int mprotect(const void *, long unsigned int, int);
#else
int mprotect(char *, unsigned int, int);
#endif
#endif

#if defined(BSD) || defined(SOLARIS) || defined(LINUX)
//KMS
// added Solaris and LINUX
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
             struct timeval *timeout);
#else
int select(int numBits, void *readFds, void *writeFds, void *exceptFds,
	struct timeval *timeout);
#endif

int socket(int, int, int);

#if defined(SUNOS) || defined(ULTRIX)
long tell(int);
int bind (int, const void*, int);
int recvfrom (int, void*, int, int, void*, int *);
int sendto (int, const void*, int, int, void*, int);
#endif
}

void CallOnUserAbort(void (*func)(int))
{
    (void)signal(SIGINT, func);
}

void Delay(int seconds)
{
    (void) sleep((unsigned) seconds);
}

void UDelay(unsigned int useconds)
{
//#ifdef SOLARIS
//   usleep(useconds_t useconds);
//#else
//   usleep(useconds);
//#endif /* SOLARIS */
}

void Abort()
{
    abort();
}

void Exit(int exitCode)
{
    exit(exitCode);
}

void RandomInit(unsigned seed)
{
    srand(seed);
}

unsigned int RandomNumber()
{
    return rand();
}

char * AllocBoundedArray(int size)
{
#ifdef NO_MPROT
    return new char[size];
#else
    int pgSize = getpagesize();
    char *ptr = new char[pgSize * 2 + size];

    mprotect(ptr, pgSize, 0);
    mprotect(ptr + pgSize + size, pgSize, 0);
    return ptr + pgSize;
#endif
}


#ifdef NO_MPROT
void DeallocBoundedArray(char *ptr, int /* size */)
{
    delete [] ptr;
}
#else
void DeallocBoundedArray(char *ptr, int size)
{
    int pgSize = getpagesize();

    mprotect(ptr - pgSize, pgSize, PROT_READ | PROT_WRITE | PROT_EXEC);
    mprotect(ptr + size, pgSize, PROT_READ | PROT_WRITE | PROT_EXEC);
    delete [] (ptr - pgSize);
}
#endif

bool
PollFile(int fd)
{
#if defined(SOLARIS) || defined(LINUX)
// KMS
    fd_set rfd,wfd,xfd;
#else
    int rfd = (1 << fd), wfd = 0, xfd = 0;
#endif
    int retVal;
    struct timeval pollTime;

#if defined(SOLARIS) || defined(LINUX)
// KMS
    FD_ZERO(&rfd);
    FD_ZERO(&wfd);
    FD_ZERO(&xfd);
    FD_SET(fd,&rfd);
#endif

// don't wait if there are no characters on the file
    pollTime.tv_sec = 0;
    pollTime.tv_usec = 0;

// poll file or socket
#if defined(BSD)
    retVal = select(32, (fd_set*)&rfd, (fd_set*)&wfd, (fd_set*)&xfd, &pollTime);
#elif defined(SOLARIS) || defined(LINUX)
    // KMS
    retVal = select(32, &rfd, &wfd, &xfd, &pollTime);
#else
    retVal = select(32, &rfd, &wfd, &xfd, &pollTime);
#endif

    ASSERT((retVal == 0) || (retVal == 1));
    if (retVal == 0)
	return FALSE;                 		// no char waiting to be read
    return TRUE;
}

int OpenForWrite(char *name)
{
    int fd = open(name, O_RDWR|O_CREAT|O_TRUNC, 0666);
    ASSERT(fd >= 0);
    return fd;
}

int OpenForReadWrite(char *name, bool crashOnError)
{
    int fd = open(name, O_RDWR, 0);
    ASSERT(!crashOnError || fd >= 0);
    return fd;
}

void Read(int fd, char *buffer, int nBytes)
{
    int retVal = read(fd, buffer, nBytes);
    ASSERT(retVal == nBytes);
}

int ReadPartial(int fd, char *buffer, int nBytes)
{
    return read(fd, buffer, nBytes);
}

void WriteFile(int fd, char *buffer, int nBytes)
{
    //printf("In sysdep.cc, nBytes: %d\n", nBytes);
	int retVal = write(fd, buffer, nBytes);
    ASSERT(retVal == nBytes);
}

void Lseek(int fd, int offset, int whence)
{
    int retVal = lseek(fd, offset, whence);
    ASSERT(retVal >= 0);
}

int Tell(int fd)
{
#if defined(BSD) || defined(SOLARIS) || defined(LINUX)
    return lseek(fd,0,SEEK_CUR); // 386BSD doesn't have the tell() system call
                                 // neither do Solaris and Linux  -KMS
#else
    return tell(fd);
#endif
}

int Close(int fd)
{
    int retVal = close(fd);
    ASSERT(retVal >= 0);
    return retVal;
}

bool Unlink(char *name)
{
    return unlink(name);
}

int OpenSocket()
{
    int sockID;

    sockID = socket(AF_UNIX, SOCK_DGRAM, 0);
    ASSERT(sockID >= 0);

    return sockID;
}

void CloseSocket(int sockID)
{
    (void) close(sockID);
}

static void InitSocketName(struct sockaddr_un *uname, char *name)
{
    uname->sun_family = AF_UNIX;
    strcpy(uname->sun_path, name);
}

void AssignNameToSocket(char *socketName, int sockID)
{
    struct sockaddr_un uName;
    int retVal;

    (void) unlink(socketName);

    InitSocketName(&uName, socketName);
    retVal = bind(sockID, (struct sockaddr *) &uName, sizeof(uName));
    ASSERT(retVal >= 0);
    DEBUG(dbgNet, "Created socket " << socketName);
}

void DeAssignNameToSocket(char *socketName)
{
    (void) unlink(socketName);
}

bool PollSocket(int sockID)
{
    return PollFile(sockID);
}

void ReadFromSocket(int sockID, char *buffer, int packetSize)
{
    int retVal;
    struct sockaddr_un uName;
#ifdef LINUX
    socklen_t size = sizeof(uName);
#else
    int size = sizeof(uName);
#endif

    retVal = recvfrom(sockID, buffer, packetSize, 0,
				   (struct sockaddr *) &uName, &size);

    if (retVal != packetSize) {
        perror("in recvfrom");
#if defined CYGWIN
	cerr << "called with " << packetSize << ", got back " << retVal
						<< ", and " << "\n";
#else
        cerr << "called with " << packetSize << ", got back " << retVal
						<< ", and " << errno << "\n";
#endif
    }
    ASSERT(retVal == packetSize);
}

void SendToSocket(int sockID, char *buffer, int packetSize, char *toName)
{
    struct sockaddr_un uName;
    int retVal;
    int retryCount;

    InitSocketName(&uName, toName);

    for(retryCount=0;retryCount < 10;retryCount++) {
      retVal = sendto(sockID, buffer, packetSize, 0,
			(struct sockaddr *) &uName, sizeof(uName));
      if (retVal == packetSize) return;
      ASSERT(retVal < 0);
      Delay(1);
    }
}
