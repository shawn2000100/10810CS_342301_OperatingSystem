// kernel.cc 
//	Initialization and cleanup routines for the Nachos kernel.
#include "copyright.h"
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

// 191012[J]: 這裡其實沒有完整實作整個kernel，這裡只實作了初始化的功能而已
// 191012[J]: 更多完整功能的定義被拆分到很多小檔案裏面: e.g., filesys.h... 

//----------------------------------------------------------------------
// Kernel::Kernel
// 	Interpret command line arguments in order to determine flags 
//	for the initialization (see also comments in main.cc)  
//----------------------------------------------------------------------
Kernel::Kernel(int argc, char **argv)
{
    randomSlice = FALSE; 
    debugUserProg = FALSE;
    consoleIn = NULL;          // default is stdin
    consoleOut = NULL;         // default is stdout
#ifndef FILESYS_STUB
    formatFlag = FALSE;
#endif
    reliability = 1;            // network reliability, default is 1.0
    hostName = 0;               // machine id, also UNIX socket name
                                // 0 is the default machine id
    // 191007[J]:-rs -ci -co...etc, 可參考 main.cc 最上面的說明
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-rs") == 0) {
 	    	ASSERT(i + 1 < argc);
	    	RandomInit(atoi(argv[i + 1]));// initialize pseudo-random
			// number generator
	    	randomSlice = TRUE;
	    	i++;
        } else if (strcmp(argv[i], "-s") == 0) {
            debugUserProg = TRUE;
		} else if (strcmp(argv[i], "-e") == 0) {
          // 191007[J]:execfile定義於kernel.h 的 char* execfile[10];
        	execfile[++execfileNum]= argv[++i];
			cout << execfile[execfileNum] << "\n";
		} else if (strcmp(argv[i], "-ci") == 0) {
	    	ASSERT(i + 1 < argc);
	    	consoleIn = argv[i + 1];
	    	i++;
		} else if (strcmp(argv[i], "-co") == 0) {
	    	ASSERT(i + 1 < argc);
	    	consoleOut = argv[i + 1];
	    	i++;
#ifndef FILESYS_STUB
		} else if (strcmp(argv[i], "-f") == 0) {
	    	formatFlag = TRUE;
#endif
        } else if (strcmp(argv[i], "-n") == 0) {
            ASSERT(i + 1 < argc);   // next argument is float
            reliability = atof(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-m") == 0) {
            ASSERT(i + 1 < argc);   // next argument is int
            hostName = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-u") == 0) {
            cout << "Partial usage: nachos [-rs randomSeed]\n";
	   		cout << "Partial usage: nachos [-s]\n";
            cout << "Partial usage: nachos [-ci consoleIn] [-co consoleOut]\n";
#ifndef FILESYS_STUB
	    	cout << "Partial usage: nachos [-nf]\n";
#endif
            cout << "Partial usage: nachos [-n #] [-m #]\n";
		}
    }
}

//----------------------------------------------------------------------
// Kernel::Initialize
// 	Initialize Nachos global data structures.  Separate from the 
//	constructor because some of these refer to earlier initialized
//	data via the "kernel" global variable.
//----------------------------------------------------------------------
void
Kernel::Initialize()
{
    // We didn't explicitly allocate the current thread we are running in.
    // But if it ever tries to give up the CPU, we better have a Thread
    // object to save its state. 

	
    currentThread = new Thread("main", threadNum++);	// 191019[J]: 主程式也是一條thread
    currentThread->setStatus(RUNNING);                // 191019[J]: 跑起來!

    stats = new Statistics();		// collect statistics
    interrupt = new Interrupt;		// start up interrupt handling
    scheduler = new Scheduler();	// initialize the ready queue
    alarm = new Alarm(randomSlice);	// start up time slicing
    machine = new Machine(debugUserProg);
    synchConsoleIn = new SynchConsoleInput(consoleIn); // input from stdin
    synchConsoleOut = new SynchConsoleOutput(consoleOut); // output to stdout
    synchDisk = new SynchDisk();    //
#ifdef FILESYS_STUB
    fileSystem = new FileSystem();
#else
    fileSystem = new FileSystem(formatFlag);
#endif // FILESYS_STUB
    postOfficeIn = new PostOfficeInput(10);
    postOfficeOut = new PostOfficeOutput(reliability);

    interrupt->Enable();
}

//----------------------------------------------------------------------
// Kernel::~Kernel
// 	Nachos is halting.  De-allocate global data structures.
//----------------------------------------------------------------------
// 1910010[J]: 解構子 不重要
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

//----------------------------------------------------------------------
// Kernel::ThreadSelfTest
//      Test threads, semaphores, synchlists
//----------------------------------------------------------------------
// 1910010[J]: 測試thread, semaphore用，可能還不用動到
void
Kernel::ThreadSelfTest() {
   Semaphore *semaphore;
   SynchList<int> *synchList;
   
   LibSelfTest();		// test library routines
   
   currentThread->SelfTest();	// test thread switching
   
   				// test semaphore operation
   semaphore = new Semaphore("test", 0);
   semaphore->SelfTest();
   delete semaphore;
   
   				// test locks, condition variables
				// using synchronized lists
   synchList = new SynchList<int>;
   synchList->SelfTest(9);
   delete synchList;

}

//----------------------------------------------------------------------
// Kernel::ConsoleTest
//      Test the synchconsole
//----------------------------------------------------------------------
// 1910010[J]: 這邊應該只是單純測試輸出入  不重要
void
Kernel::ConsoleTest() {
    char ch;

    cout << "Testing the console device.\n" 
        << "Typed characters will be echoed, until ^D is typed.\n"
        << "Note newlines are needed to flush input through UNIX.\n";
    cout.flush();

    do {
        ch = synchConsoleIn->GetChar();
        if(ch != EOF) synchConsoleOut->PutChar(ch);   // echo it!
    } while (ch != EOF);

    cout << "\n";

}

//----------------------------------------------------------------------
// Kernel::NetworkTest
//      Test whether the post office is working. On machines #0 and #1, do:
//
//      1. send a message to the other machine at mail box #0
//      2. wait for the other machine's message to arrive (in our mailbox #0)
//      3. send an acknowledgment for the other machine's message
//      4. wait for an acknowledgement from the other machine to our 
//          original message
//
//  This test works best if each Nachos machine has its own window
//----------------------------------------------------------------------
// 1910010[J]: 測試網路，不用動到
void
Kernel::NetworkTest() {

    if (hostName == 0 || hostName == 1) {
        // if we're machine 1, send to 0 and vice versa
        int farHost = (hostName == 0 ? 1 : 0); 
        PacketHeader outPktHdr, inPktHdr;
        MailHeader outMailHdr, inMailHdr;
        char *data = "Hello there!";
        char *ack = "Got it!";
        char buffer[MaxMailSize];

        // construct packet, mail header for original message
        // To: destination machine, mailbox 0
        // From: our machine, reply to: mailbox 1
        outPktHdr.to = farHost;         
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.length = strlen(data) + 1;

        // Send the first message
        postOfficeOut->Send(outPktHdr, outMailHdr, data); 

        // Wait for the first message from the other machine
        postOfficeIn->Receive(0, &inPktHdr, &inMailHdr, buffer);
        cout << "Got: " << buffer << " : from " << inPktHdr.from << ", box " 
                                                << inMailHdr.from << "\n";
        cout.flush();

        // Send acknowledgement to the other machine (using "reply to" mailbox
        // in the message that just arrived
        outPktHdr.to = inPktHdr.from;
        outMailHdr.to = inMailHdr.from;
        outMailHdr.length = strlen(ack) + 1;
        postOfficeOut->Send(outPktHdr, outMailHdr, ack); 

        // Wait for the ack from the other machine to the first message we sent
	postOfficeIn->Receive(1, &inPktHdr, &inMailHdr, buffer);
        cout << "Got: " << buffer << " : from " << inPktHdr.from << ", box " 
                                                << inMailHdr.from << "\n";
        cout.flush();
    }

    // Then we're done!
}

// 1910010[J]: ?
void ForkExecute(Thread *t)
{
	if ( !t->space->Load(t->getName()) ) {
    	return;             // executable not found
    }
	
    t->space->Execute(t->getName());

}

// 1910010[J]: 依序執行全部的thread...?
void Kernel::ExecAll()
{
	for (int i=1;i<=execfileNum;i++) {
		int a = Exec(execfile[i]);
	}
	currentThread->Finish();
    //Kernel::Exec();	// 1910010[J]: 註解掉的原因是...?
}

// 1910010[J]: 創造一條 thread 並分配資源?
int Kernel::Exec(char* name)
{
	t[threadNum] = new Thread(name, threadNum);
	t[threadNum]->space = new AddrSpace();
	t[threadNum]->Fork((VoidFunctionPtr) &ForkExecute, (void *)t[threadNum]);
	threadNum++;

	return threadNum-1;
// 1910010[J]: 這邊可能是以後要練習multi-thread才會用到的
/*
    cout << "Total threads number is " << execfileNum << endl;
    for (int n=1;n<=execfileNum;n++) {
		t[n] = new Thread(execfile[n]);
		t[n]->space = new AddrSpace();
		t[n]->Fork((VoidFunctionPtr) &ForkExecute, (void *)t[n]);
		cout << "Thread " << execfile[n] << " is executing." << endl;
	}
	cout << "debug Kernel::Run finished.\n";	
*/
//  Thread *t1 = new Thread(execfile[1]);
//  Thread *t1 = new Thread("../test/test1");
//  Thread *t2 = new Thread("../test/test2");

//    AddrSpace *halt = new AddrSpace();
//  t1->space = new AddrSpace();
//  t2->space = new AddrSpace();

//    halt->Execute("../test/halt");
//  t1->Fork((VoidFunctionPtr) &ForkExecute, (void *)t1);
//  t2->Fork((VoidFunctionPtr) &ForkExecute, (void *)t2);

//	currentThread->Finish();
//    Kernel::Run();
//  cout << "after ThreadedKernel:Run();" << endl;  // unreachable
}
