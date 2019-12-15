#include "debug.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "alarm.h"
#include "filesys.h"
#include "machine.h"

class PostOfficeInput;
class PostOfficeOutput;
class SynchConsoleInput;
class SynchConsoleOutput;
class SynchDisk;

typedef int OpenFileId;

class Kernel {
  public:
    Kernel(int argc, char **argv);
    ~Kernel();

    void Initialize();
    void ExecAll();
    int Exec(char* name);
    Thread* getThread(int threadID){return t[threadID];}

    void PrintInt(int number);
    int CreateFile(char* filename);
    OpenFileId OpenFile(char* name);
    int WriteFile(char* buffer, int size, OpenFileId id);
    int ReadFile(char* buffer, int size, OpenFileId id);
    int CloseFile(OpenFileId id);

    Thread *currentThread;

    Scheduler *scheduler;

    Interrupt *interrupt;

    Statistics *stats;

    Alarm *alarm;

    Machine *machine;

    SynchConsoleInput *synchConsoleIn;

    SynchConsoleOutput *synchConsoleOut;

    SynchDisk *synchDisk;

    FileSystem *fileSystem;

    PostOfficeInput *postOfficeIn;

    PostOfficeOutput *postOfficeOut;

    int hostName;

  private:
	Thread* t[10];
	char*   execfile[10];
	int execfileNum;
	int threadNum;
    bool randomSlice;
    bool debugUserProg;
    double reliability;
    char *consoleIn;
    char *consoleOut;
};
