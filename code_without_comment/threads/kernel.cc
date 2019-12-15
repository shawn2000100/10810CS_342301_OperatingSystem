#include "debug.h"
#include "main.h"
#include "kernel.h"
#include "sysdep.h"
#include "synch.h"
#include "synchlist.h"
#include "libtest.h"
#include "string.h"
#include "synchdisk.h"
#include "post.h"
#include "synchconsole.h"

Kernel::Kernel(int argc, char **argv)
{
    randomSlice = FALSE;
    debugUserProg = FALSE;
    consoleIn = NULL;
    consoleOut = NULL;
    reliability = 1;
    hostName = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-rs") == 0) {
 	    	ASSERT(i + 1 < argc);
	    	RandomInit(atoi(argv[i + 1]));
	    	randomSlice = TRUE;
	    	i++;
        }
        else if (strcmp(argv[i], "-s") == 0) {
            debugUserProg = TRUE;
		}
		else if (strcmp(argv[i], "-e") == 0) {
        	execfile[++execfileNum]= argv[i];
			cout << execfile[execfileNum] << "\n";
			i++;
		}
		else if (strcmp(argv[i], "-ci") == 0) {
	    	ASSERT(i + 1 < argc);
	    	consoleIn = argv[i + 1];
	    	i++;
		}
		else if (strcmp(argv[i], "-co") == 0) {
	    	ASSERT(i + 1 < argc);
	    	consoleOut = argv[i + 1];
	    	i++;
		}
		else if (strcmp(argv[i], "-n") == 0) {
            ASSERT(i + 1 < argc);
            reliability = atof(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-m") == 0) {
            ASSERT(i + 1 < argc);
            hostName = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-u") == 0) {
            cout << "Partial usage: nachos [-rs randomSeed]\n";
	   		cout << "Partial usage: nachos [-s]\n";
            cout << "Partial usage: nachos [-ci consoleIn] [-co consoleOut]\n";
            cout << "Partial usage: nachos [-n #] [-m #]\n";
		}
    }
}

void Kernel::Initialize()
{
    currentThread = new Thread("main", threadNum++);

    currentThread->setStatus(RUNNING);

    stats = new Statistics();

    interrupt = new Interrupt;

    scheduler = new Scheduler();

    alarm = new Alarm(randomSlice);

    machine = new Machine(debugUserProg);

    synchConsoleIn = new SynchConsoleInput(consoleIn);

    synchConsoleOut = new SynchConsoleOutput(consoleOut);

    synchDisk = new SynchDisk();

    postOfficeIn = new PostOfficeInput(10);

    postOfficeOut = new PostOfficeOutput(reliability);

    interrupt->Enable();
}

Kernel::~Kernel()
{
    delete stats;
    delete interrupt;
    delete scheduler;
    delete alarm;
    delete machine;
    delete synchConsoleIn;
    delete synchConsoleOut;
    delete synchDisk;
    delete fileSystem;
    delete postOfficeIn;
    delete postOfficeOut;
    Exit(0);
}

void ForkExecute(Thread *t)
{
	if ( !t->space->Load(t->getName()) ) {
    	return;
    }
    t->space->Execute(t->getName());
}

void Kernel::ExecAll()
{
	for (int i=1;i<=execfileNum;i++) {
		int a = Exec(execfile[i]);
	}
	currentThread->Finish();
}

int Kernel::Exec(char* name)
{
	t[threadNum] = new Thread(name, threadNum);
	t[threadNum]->space = new AddrSpace();
	t[threadNum]->Fork((VoidFunctionPtr) &ForkExecute, (void *)t[threadNum]);
	threadNum++;
	return threadNum-1;
}
