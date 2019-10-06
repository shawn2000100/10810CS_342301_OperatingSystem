// console.h 
//	Data structures to simulate the behavior of a terminal
//	I/O device.  A terminal has two parts -- a keyboard input,
//	and a display output, each of which produces/accepts 
//	characters sequentially.
//
//	The console hardware device is asynchronous.  When a character is
//	written to the device, the routine returns immediately, and an 
//	interrupt handler is called later when the I/O completes.
//	For reads, an interrupt handler is called when a character arrives. 
//
//	In either case, the serial line connecting the computer
//	to the console has limited bandwidth (like a modem!), and so
//	each character takes measurable time.
//
//	The user of the device registers itself to be called "back" when 
//	the read/write interrupts occur.  There is a separate interrupt
//	for read and write, and the device is "duplex" -- a character
//	can be outgoing and incoming at the same time.
//
//  DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef CONSOLE_H
#define CONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "callback.h"

// The following two classes define the input (and output) side of a 
// hardware console device.  Input (and output) to the device is simulated 
// by reading (and writing) to the UNIX file "readFile" (and "writeFile").
//
// Since input (and output) to the device is asynchronous, the interrupt 
// handler "callWhenAvail" is called when a character has arrived to be 
// read in (and "callWhenDone" is called when an output character has been 
// "put" so that the next character can be written).
//
// In practice, usually a single hardware thing that does both
// serial input and serial output.  But conceptually simpler to
// use two objects.

class ConsoleInput : public CallBackObj {
  public:
    ConsoleInput(char *readFile, CallBackObj *toCall);
				// initialize hardware console input 
    ~ConsoleInput();		// clean up console emulation

    char GetChar();	   	// Poll the console input.  If a char is 
				// available, return it.  Otherwise, return EOF.
    				// "callWhenAvail" is called whenever there is 
				// a char to be gotten

    void CallBack();		// Invoked when a character arrives
				// from the keyboard.

  private:
    int readFileNo;			// UNIX file emulating the keyboard 
    CallBackObj *callWhenAvail;		// Interrupt handler to call when 
					// there is a char to be read
    char incoming;    			// Contains the character to be read,
					// if there is one available. 
					// Otherwise contains EOF.
};

class ConsoleOutput : public CallBackObj {
  public:
    ConsoleOutput(char *writeFile, CallBackObj *toCall);
				// initialize hardware console output 
    ~ConsoleOutput();		// clean up console emulation

    void PutChar(char ch);	// Write "ch" to the console display, 
				// and return immediately.  "callWhenDone" 
				// will called when the I/O completes. 
    void CallBack();		// Invoked when next character can be put
				// out to the display.
    void PutInt(int n);         // Write n to the console display 

  private:
    int writeFileNo;			// UNIX file emulating the display
    CallBackObj *callWhenDone;		// Interrupt handler to call when 
					// the next char can be put 
    bool putBusy;    			// Is a PutChar operation in progress?
					// If so, you can't do another one!
};

#endif // CONSOLE_H
