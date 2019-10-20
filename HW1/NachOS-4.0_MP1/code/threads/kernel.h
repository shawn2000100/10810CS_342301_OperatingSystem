// kernel.h
//	Global variables for the Nachos kernel.
// 
#ifndef KERNEL_H
#define KERNEL_H

#include "copyright.h"
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
    				// Interpret command line arguments
    ~Kernel();		        // deallocate the kernel
    
    void Initialize(); 		// initialize the kernel -- separated
				// from constructor because 
				// refers to "kernel" as a global
    void ExecAll();
    int Exec(char* name); // 1910010[J]: 創造一條thread並分配資源
    void ThreadSelfTest();	// self test of threads and synchronization
	
    void ConsoleTest();         // interactive console self test
    void NetworkTest();         // interactive 2-machine network test
    Thread* getThread(int threadID){return t[threadID];}    


    void PrintInt(int number); 	
    int CreateFile(char* filename); // fileSystem call
    
    // 1910010[J]: 似乎syscall.h裡面定義的function也要在kernel.h中宣告? 但這邊已經幫我們宣告好了，故不用更動
    // 1910010[J]: 這邊的4個function與filesys.h的4個function互相對照，實作的部分可能就是被定義在filesys.h那邊
    // 191019[J]:  呼叫程序應該是這樣 exception.cc --> ksyscall.h --> kernel --> 各種實作 (e.g., filesys.h)
    OpenFileId OpenFile(char* name);                       
    int WriteFile(char* buffer, int size, OpenFileId id); 
    int ReadFile(char* buffer, int size, OpenFileId id);  
    int CloseFile(OpenFileId id);  // 191012[J]: kernel.h這邊定義了介面，而真正的實作則是在filesys.h裡面，可使用grep -nr CloseFile 來查驗

// These are public for notational convenience; really, 
// they're global variables used everywhere.

    Thread *currentThread;	// the thread holding the CPU
    Scheduler *scheduler;	// the ready list
    Interrupt *interrupt;	// interrupt status
    Statistics *stats;		// performance metrics
    Alarm *alarm;		// the software alarm clock    
    Machine *machine;           // the simulated CPU
    SynchConsoleInput *synchConsoleIn;
    SynchConsoleOutput *synchConsoleOut;
    SynchDisk *synchDisk;
    FileSystem *fileSystem;     
    PostOfficeInput *postOfficeIn;
    PostOfficeOutput *postOfficeOut;

    int hostName;               // machine identifier

  private:

	Thread* t[10]; // 1910010[J]: 在其他地方看到的 t 代表是thread的意思
	char*   execfile[10]; 
	int execfileNum;
	int threadNum;
    bool randomSlice;		// enable pseudo-random time slicing
    bool debugUserProg;         // single step user program
    double reliability;         // likelihood messages are dropped
    char *consoleIn;            // file to read console input from
    char *consoleOut;           // file to send console output to
#ifndef FILESYS_STUB
    bool formatFlag;          // format the disk if this is true
#endif
};


#endif // KERNEL_H


