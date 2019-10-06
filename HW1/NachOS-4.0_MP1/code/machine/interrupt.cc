// interrupt.cc 
//	Routines to simulate hardware interrupts.
//
//	The hardware provides a routine (SetLevel) to enable or disable
//	interrupts.
//
//	In order to emulate the hardware, we need to keep track of all
//	interrupts the hardware devices would cause, and when they
//	are supposed to occur.  
//
//	This module also keeps track of simulated time.  Time advances
//	only when the following occur: 
//		interrupts are re-enabled
//		a user instruction is executed
//		there is nothing in the ready queue
//
//  DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "interrupt.h"
#include "main.h"

// String definitions for debugging messages

static char *intLevelNames[] = { "off", "on"};
static char *intTypeNames[] = { "timer", "disk", "console write", 
			"console read", "network send", 
			"network recv"};

//----------------------------------------------------------------------
// PendingInterrupt::PendingInterrupt
// 	Initialize a hardware device interrupt that is to be scheduled 
//	to occur in the near future.
//
//	"callOnInt" is the object to call when the interrupt occurs
//	"time" is when (in simulated time) the interrupt is to occur
//	"kind" is the hardware device that generated the interrupt
//----------------------------------------------------------------------

PendingInterrupt::PendingInterrupt(CallBackObj *callOnInt, 
					int time, IntType kind)
{
    callOnInterrupt = callOnInt;
    when = time;
    type = kind;
}

//----------------------------------------------------------------------
// PendingCompare
//	Compare to interrupts based on which should occur first.
//----------------------------------------------------------------------

static int
PendingCompare (PendingInterrupt *x, PendingInterrupt *y)
{
    if (x->when < y->when) { return -1; }
    else if (x->when > y->when) { return 1; }
    else { return 0; }
}

//----------------------------------------------------------------------
// Interrupt::Interrupt
// 	Initialize the simulation of hardware device interrupts.
//	
//	Interrupts start disabled, with no interrupts pending, etc.
//----------------------------------------------------------------------

Interrupt::Interrupt()
{
    level = IntOff;
    pending = new SortedList<PendingInterrupt *>(PendingCompare);
    inHandler = FALSE;
    yieldOnReturn = FALSE;
    status = SystemMode;
}

//----------------------------------------------------------------------
// Interrupt::~Interrupt
// 	De-allocate the data structures needed by the interrupt simulation.
//----------------------------------------------------------------------

Interrupt::~Interrupt()
{
    while (!pending->IsEmpty()) {
	delete pending->RemoveFront();
    }
    delete pending;
}

//----------------------------------------------------------------------
// Interrupt::ChangeLevel
// 	Change interrupts to be enabled or disabled, without advancing 
//	the simulated time (normally, enabling interrupts advances the time).
//
//	Used internally.
//
//	"old" -- the old interrupt status
//	"now" -- the new interrupt status
//----------------------------------------------------------------------

void
Interrupt::ChangeLevel(IntStatus old, IntStatus now)
{
    level = now;
    DEBUG(dbgInt, "\tinterrupts: " << intLevelNames[old] << " -> " << intLevelNames[now]);
}

//----------------------------------------------------------------------
// Interrupt::SetLevel
// 	Change interrupts to be enabled or disabled, and if interrupts
//	are being enabled, advance simulated time by calling OneTick().
//
// Returns:
//	The old interrupt status.
// Parameters:
//	"now" -- the new interrupt status
//----------------------------------------------------------------------

IntStatus
Interrupt::SetLevel(IntStatus now)
{
    IntStatus old = level;
    
    // interrupt handlers are prohibited from enabling interrupts
    ASSERT((now == IntOff) || (inHandler == FALSE));

    ChangeLevel(old, now);			// change to new state
    if ((now == IntOn) && (old == IntOff)) {
	OneTick();				// advance simulated time
    }
    return old;
}

//----------------------------------------------------------------------
// Interrupt::OneTick
// 	Advance simulated time and check if there are any pending 
//	interrupts to be called. 
//
//	Two things can cause OneTick to be called:
//		interrupts are re-enabled
//		a user instruction is executed
//----------------------------------------------------------------------
void
Interrupt::OneTick()
{
    MachineStatus oldStatus = status;
    Statistics *stats = kernel->stats;

// advance simulated time
    if (status == SystemMode) {
        stats->totalTicks += SystemTick;
	stats->systemTicks += SystemTick;
    } else {
	stats->totalTicks += UserTick;
	stats->userTicks += UserTick;
    }
    DEBUG(dbgInt, "== Tick " << stats->totalTicks << " ==");

// check any pending interrupts are now ready to fire
    ChangeLevel(IntOn, IntOff);	// first, turn off interrupts
				// (interrupt handlers run with
				// interrupts disabled)
    CheckIfDue(FALSE);		// check for pending interrupts
    ChangeLevel(IntOff, IntOn);	// re-enable interrupts
    if (yieldOnReturn) {	// if the timer device handler asked 
    				// for a context switch, ok to do it now
	yieldOnReturn = FALSE;
 	status = SystemMode;		// yield is a kernel routine
	kernel->currentThread->Yield();
	status = oldStatus;
    }
}

//----------------------------------------------------------------------
// Interrupt::YieldOnReturn
// 	Called from within an interrupt handler, to cause a context switch
//	(for example, on a time slice) in the interrupted thread,
//	when the handler returns.
//
//	We can't do the context switch here, because that would switch
//	out the interrupt handler, and we want to switch out the 
//	interrupted thread.
//----------------------------------------------------------------------

void
Interrupt::YieldOnReturn()
{ 
    ASSERT(inHandler == TRUE);  
    yieldOnReturn = TRUE; 
}

//----------------------------------------------------------------------
// Interrupt::Idle
// 	Routine called when there is nothing in the ready queue.
//
//	Since something has to be running in order to put a thread
//	on the ready queue, the only thing to do is to advance 
//	simulated time until the next scheduled hardware interrupt.
//
//	If there are no pending interrupts, stop.  There's nothing
//	more for us to do.
//----------------------------------------------------------------------
void
Interrupt::Idle()
{
    DEBUG(dbgInt, "Machine idling; checking for interrupts.");
    status = IdleMode;
	DEBUG(dbgTraCode, "In Interrupt::Idle, into CheckIfDue, " << kernel->stats->totalTicks);
    if (CheckIfDue(TRUE)) {	// check for any pending interrupts
	DEBUG(dbgTraCode, "In Interrupt::Idle, return true from CheckIfDue, " << kernel->stats->totalTicks);
	status = SystemMode;
	return;			// return in case there's now
				// a runnable thread
    }
	DEBUG(dbgTraCode, "In Interrupt::Idle, return false from CheckIfDue, " << kernel->stats->totalTicks);

    // if there are no pending interrupts, and nothing is on the ready
    // queue, it is time to stop.   If the console or the network is 
    // operating, there are *always* pending interrupts, so this code
    // is not reached.  Instead, the halt must be invoked by the user program.

    DEBUG(dbgInt, "Machine idle.  No interrupts to do.");
    cout << "No threads ready or runnable, and no pending interrupts.\n";
    cout << "Assuming the program completed.\n";
    Halt();
}

//----------------------------------------------------------------------
// Interrupt::Halt
// 	Shut down Nachos cleanly, printing out performance statistics.
//----------------------------------------------------------------------
void
Interrupt::Halt()
{
    cout << "Machine halting!\n\n";
    cout << "This is halt\n";
    kernel->stats->Print();
    delete kernel;	// Never returns.
}
/*
void 
Interrupt::PrintInt(int number)
{
    kernel->PrintInt(number);
    //kernel->synchConsoleOut->consoleOutput->PutInt(number);   
}

int
Interrupt::CreateFile(char *filename)
{
    return kernel->CreateFile(filename);
}

OpenFileId
Interrupt::OpenFile(char *name)
{
    return kernel->OpenFile(name);
}

int
Interrupt::WriteFile(char *buffer, int size, OpenFileId id)
{
    return kernel->WriteFile(buffer, size, id);
}

int
Interrupt::ReadFile(char *buffer, int size, OpenFileId id)
{
    return kernel->ReadFile(buffer, size, id);
}

int
Interrupt::CloseFile(OpenFileId id)
{
        return kernel->CloseFile(id);
}
*/
//----------------------------------------------------------------------
// Interrupt::Schedule
// 	Arrange for the CPU to be interrupted when simulated time
//	reaches "now + when".
//
//	Implementation: just put it on a sorted list.
//
//	NOTE: the Nachos kernel should not call this routine directly.
//	Instead, it is only called by the hardware device simulators.
//
//	"toCall" is the object to call when the interrupt occurs
//	"fromNow" is how far in the future (in simulated time) the 
//		 interrupt is to occur
//	"type" is the hardware device that generated the interrupt
//----------------------------------------------------------------------
void
Interrupt::Schedule(CallBackObj *toCall, int fromNow, IntType type)
{
    int when = kernel->stats->totalTicks + fromNow;
    PendingInterrupt *toOccur = new PendingInterrupt(toCall, when, type);

    DEBUG(dbgInt, "Scheduling interrupt handler the " << intTypeNames[type] << " at time = " << when);
    ASSERT(fromNow > 0);

    pending->Insert(toOccur);
}

//----------------------------------------------------------------------
// Interrupt::CheckIfDue
// 	Check if any interrupts are scheduled to occur, and if so, 
//	fire them off.
//
// Returns:
//	TRUE, if we fired off any interrupt handlers
// Params:
//	"advanceClock" -- if TRUE, there is nothing in the ready queue,
//		so we should simply advance the clock to when the next 
//		pending interrupt would occur (if any).
//----------------------------------------------------------------------
bool
Interrupt::CheckIfDue(bool advanceClock)
{
    PendingInterrupt *next;
    Statistics *stats = kernel->stats;

    ASSERT(level == IntOff);		// interrupts need to be disabled,
					// to invoke an interrupt handler
    if (debug->IsEnabled(dbgInt)) {
	DumpState();
    }
    if (pending->IsEmpty()) {   	// no pending interrupts
	return FALSE;	
    }		
    next = pending->Front();

    if (next->when > stats->totalTicks) {
        if (!advanceClock) {		// not time yet
            return FALSE;
        }
        else {      		// advance the clock to next interrupt
	    stats->idleTicks += (next->when - stats->totalTicks);
	    stats->totalTicks = next->when;
	    // UDelay(1000L); // rcgood - to stop nachos from spinning.
	}
    }

    DEBUG(dbgInt, "Invoking interrupt handler for the ");
    DEBUG(dbgInt, intTypeNames[next->type] << " at time " << next->when);

    if (kernel->machine != NULL) {
    	kernel->machine->DelayedLoad(0, 0);
    }

    inHandler = TRUE;
    do {
        next = pending->RemoveFront();    // pull interrupt off list
		DEBUG(dbgTraCode, "In Interrupt::CheckIfDue, into callOnInterrupt->CallBack, " << stats->totalTicks);
        next->callOnInterrupt->CallBack();// call the interrupt handler
		DEBUG(dbgTraCode, "In Interrupt::CheckIfDue, return from callOnInterrupt->CallBack, " << stats->totalTicks);
	delete next;
    } while (!pending->IsEmpty() 
    		&& (pending->Front()->when <= stats->totalTicks));
    inHandler = FALSE;
    return TRUE;
}

//----------------------------------------------------------------------
// PrintPending
// 	Print information about an interrupt that is scheduled to occur.
//	When, where, why, etc.
//----------------------------------------------------------------------

static void
PrintPending (PendingInterrupt *pending)
{
    cout << "Interrupt handler "<< intTypeNames[pending->type];
    cout << ", scheduled at " << pending->when;
}

//----------------------------------------------------------------------
// DumpState
// 	Print the complete interrupt state - the status, and all interrupts
//	that are scheduled to occur in the future.
//----------------------------------------------------------------------

void
Interrupt::DumpState()
{
    cout << "Time: " << kernel->stats->totalTicks;
    cout << ", interrupts " << intLevelNames[level] << "\n";
    cout << "Pending interrupts:\n";
    pending->Apply(PrintPending);
    cout << "\nEnd of pending interrupts\n";
}


