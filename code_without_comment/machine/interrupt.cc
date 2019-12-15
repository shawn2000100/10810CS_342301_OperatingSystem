#include "interrupt.h"
#include "main.h"

static char *intLevelNames[] = {"off", "on"};

static char *intTypeNames[] = { "timer", "disk", "console write",
                                "console read", "network send",
                                "network recv"};

PendingInterrupt::PendingInterrupt(CallBackObj *callOnInt, int time, IntType kind)
{
    callOnInterrupt = callOnInt;
    when = time;
    type = kind;
}

static int PendingCompare (PendingInterrupt *x, PendingInterrupt *y)
{
    if (x->when < y->when) { return -1; }
    else if (x->when > y->when) { return 1; }
    else { return 0; }
}

Interrupt::Interrupt()
{
    level = IntOff;
    pending = new SortedList<PendingInterrupt *>(PendingCompare);
    inHandler = FALSE;
    yieldOnReturn = FALSE;
    status = SystemMode;
}

Interrupt::~Interrupt()
{
    while (!pending->IsEmpty()) {
	delete pending->RemoveFront();
    }
    delete pending;
}

void Interrupt::ChangeLevel(IntStatus old, IntStatus now)
{
    level = now;
}

IntStatus Interrupt::SetLevel(IntStatus now)
{
    IntStatus old = level;

    ASSERT((now == IntOff) || (inHandler == FALSE));

    ChangeLevel(old, now);

    if ((now == IntOn) && (old == IntOff)) {
        OneTick();
    }
    return old;
}

void Interrupt::OneTick()
{
    MachineStatus oldStatus = status;
    Statistics *stats = kernel->stats;

    if (status == SystemMode) {
        stats->totalTicks += SystemTick;
	stats->systemTicks += SystemTick;
    } else {
	stats->totalTicks += UserTick;
	stats->userTicks += UserTick;
    }

    ChangeLevel(IntOn, IntOff);
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

void Interrupt::YieldOnReturn()
{
    ASSERT(inHandler == TRUE);
    yieldOnReturn = TRUE;
}

void Interrupt::Idle()
{
    status = IdleMode;
	if (CheckIfDue(TRUE)) {	// check for any pending interrupts
	status = SystemMode;
	return;			// return in case there's now
				// a runnable thread
    }

    Halt();
}

void Interrupt::Halt()
{
    kernel->stats->Print();
    delete kernel;	// Never returns.
}

void Interrupt::Schedule(CallBackObj *toCall, int fromNow, IntType type)
{
    int when = kernel->stats->totalTicks + fromNow;
    PendingInterrupt *toOccur = new PendingInterrupt(toCall, when, type);

    ASSERT(fromNow > 0);

    pending->Insert(toOccur); // 1910010[J]: Register interrupt callback function in pending queue
}

bool Interrupt::CheckIfDue(bool advanceClock)
{
    PendingInterrupt *next;
    Statistics *stats = kernel->stats;

    ASSERT(level == IntOff);

    if (debug->IsEnabled(dbgInt)) {
    DumpState();
    }

    if (pending->IsEmpty()) {
      return FALSE;
    }

    next = pending->Front();

    if (next->when > stats->totalTicks) {
        if (!advanceClock) {
            return FALSE;
        }
        else {
	        stats->idleTicks += (next->when - stats->totalTicks);
	        stats->totalTicks = next->when;
       // UDelay(1000L); // rcgood - to stop nachos from spinning.
	      }
    }

    if (kernel->machine != NULL) {
    	kernel->machine->DelayedLoad(0, 0);
    }

    inHandler = TRUE;

    do {
        next = pending->RemoveFront();
        next->callOnInterrupt->CallBack();
        delete next;
    } while ( !pending->IsEmpty() && (pending->Front()->when <= stats->totalTicks) );

    inHandler = FALSE;
    return TRUE;
}

static void PrintPending (PendingInterrupt *pending)
{
    cout << "Interrupt handler "<< intTypeNames[pending->type];
    cout << ", scheduled at " << pending->when;
}

void Interrupt::DumpState()
{
    cout << "Time: " << kernel->stats->totalTicks;
    cout << ", interrupts " << intLevelNames[level] << "\n";
    cout << "Pending interrupts:\n";
    pending->Apply(PrintPending);
    cout << "\nEnd of pending interrupts\n";
}


