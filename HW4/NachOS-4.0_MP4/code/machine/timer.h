// timer.h 
//	Data structures to emulate a hardware timer.
//
//	A hardware timer generates a CPU interrupt every X milliseconds.
//	This means it can be used for implementing time-slicing, or for
//	having a thread go to sleep for a specific period of time. 
//
//	We emulate a hardware timer by scheduling an interrupt to occur
//	every time stats->totalTicks has increased by TimerTicks.
//
//	In order to introduce some randomness into time-slicing, if "doRandom"
//	is set, then the interrupt comes after a random number of ticks.
//
//  DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef TIMER_H
#define TIMER_H

#include "copyright.h"
#include "utility.h"
#include "callback.h"

// The following class defines a hardware timer. 
class Timer : public CallBackObj {
  public:
    Timer(bool doRandom, CallBackObj *toCall);
				// Initialize the timer, and callback to "toCall"
				// every time slice.
    virtual ~Timer() {}
    
    void Disable() { disable = TRUE; }
    				// Turn timer device off, so it doesn't
				// generate any more interrupts.

  private:
    bool randomize;		// set if we need to use a random timeout delay
    CallBackObj *callPeriodically; // call this every TimerTicks time units 
    bool disable;		// turn off the timer device after next
    				// interrupt.
    
    void CallBack();		// called internally when the hardware
				// timer generates an interrupt

    void SetInterrupt();  	// cause an interrupt to occur in the
    				// the future after a fixed or random
				// delay
};

#endif // TIMER_H
