#include "utility.h"
#include "sysdep.h"
#include "machine.h"
#include "addrspace.h"

#define MachineStateSize 75

const int StackSize = (8 * 1024);

enum ThreadStatus { JUST_CREATED, RUNNING, READY, BLOCKED, ZOMBIE };

class Thread {
  private:
    int *stackTop;
    void *machineState[MachineStateSize];

  public:
    Thread(char* debugName, int threadID);
    ~Thread();

    void Fork(VoidFunctionPtr func, void *arg);

    void Yield();

    void Sleep(bool finishing);

    void Begin();

    void Finish();

    void CheckOverflow();

    void setStatus(ThreadStatus st) { status = st; }

    ThreadStatus getStatus() { return (status); }

	char* getName() { return (name); }

	int getID() { return (ID); }
    void Print() { cout << name; }
    void SelfTest();

  private:
    int *stack;
    ThreadStatus status;
    char* name;
	int   ID;
    void StackAllocate(VoidFunctionPtr func, void *arg);
    int userRegisters[NumTotalRegs];

  public:
    void SaveUserState();
    void RestoreUserState();

    AddrSpace *space;
};

extern void ThreadPrint(Thread *thread);

extern "C" {
    void ThreadRoot();
    void SWITCH(Thread *oldThread, Thread *newThread);
}
