#include "thread.h"
#include "list.h"
#include "main.h"

class Semaphore {
  public:
    Semaphore(char* debugName, int initialValue);	// set initial value
    ~Semaphore();   					// de-allocate semaphore
    char* getName() { return name;}			// debugging assist

    void P();	 	// these are the only operations on a semaphore
    void V();	 	// they are both *atomic*
    void SelfTest();	// test routine for semaphore implementation

  private:
    char* name;        // useful for debugging
    int value;         // semaphore value, always >= 0
    List<Thread *> *queue;
		  	// threads waiting in P() for the value to be > 0
   };

class Lock {
  public:
    Lock(char* debugName);  	// initialize lock to be FREE
    ~Lock();			// deallocate lock
    char* getName() { return name; }	// debugging assist

    void Acquire(); 		// these are the only operations on a lock
    void Release(); 		// they are both *atomic*

    bool IsHeldByCurrentThread() {
    		return lockHolder == kernel->currentThread; }

  private:
    char *name;			// debugging assist
    Thread *lockHolder;		// thread currently holding lock
    Semaphore *semaphore;	// we use a semaphore to implement lock
};

class Condition {
  public:
    Condition(char* debugName);	// initialize condition to
					// "no one waiting"
    ~Condition();			// deallocate the condition
    char* getName() { return (name); }

    void Wait(Lock *conditionLock); 	// these are the 3 operations on
					// condition variables; releasing the
					// lock and going to sleep are
					// *atomic* in Wait()
    void Signal(Lock *conditionLock);   // conditionLock must be held by
    void Broadcast(Lock *conditionLock);// the currentThread for all of
					// these operations
    // SelfTest routine provided by SyncLists

  private:
    char* name;
    List<Semaphore *> *waitQueue;	// list of waiting threads
};
