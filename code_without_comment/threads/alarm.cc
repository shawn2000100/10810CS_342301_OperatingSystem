#include "alarm.h"
#include "main.h"

Alarm::Alarm(bool doRandom)
{
    timer = new Timer(doRandom, this);
}

void
Alarm::CallBack()
{
    Interrupt *interrupt = kernel->interrupt;
    MachineStatus status = interrupt->getStatus();

    if (status != IdleMode) {
	interrupt->YieldOnReturn();
    }
}
