#include "list.h"
#include "thread.h"

class Scheduler {
  public:
    Scheduler();		// Initialize list of ready threads
    ~Scheduler();		// De-allocate ready list

    void ReadyToRun(Thread* thread);
    				// Thread can be dispatched.
    Thread* FindNextToRun();	// Dequeue first thread on the ready
				// list, if any, and return thread.
    void Run(Thread* nextThread, bool finishing);
    				// Cause nextThread to start running
    void CheckToBeDestroyed();// Check if thread that had been
    				// running needs to be deleted
    void Print();		// Print contents of ready list

  private:
    List<Thread *> *readyList;

    Thread *toBeDestroyed;
};
