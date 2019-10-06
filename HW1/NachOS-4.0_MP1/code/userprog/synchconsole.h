// synchconsole.h 
//	Data structures for synchronized access to the keyboard
//	and console display devices.
//
//	NOTE: this abstraction is not completely implemented.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "callback.h"
#include "console.h"
#include "synch.h"

// The following two classes define synchronized input and output to
// a console device

class SynchConsoleInput : public CallBackObj {
  public:
    SynchConsoleInput(char *inputFile); // Initialize the console device
    ~SynchConsoleInput();		// Deallocate console device

    char GetChar();		// Read a character, waiting if necessary
    
  private:
    ConsoleInput *consoleInput;	// the hardware keyboard
    Lock *lock;			// only one reader at a time
    Semaphore *waitFor;		// wait for callBack

    void CallBack();		// called when a keystroke is available
};

class SynchConsoleOutput : public CallBackObj {
  public:
    SynchConsoleOutput(char *outputFile); // Initialize the console device
    ~SynchConsoleOutput();

    void PutChar(char ch);	// Write a character, waiting if necessary
    
    void PutInt(int n);
   
  private:
    ConsoleOutput *consoleOutput;// the hardware display
    Lock *lock;			// only one writer at a time
    Semaphore *waitFor;		// wait for callBack

    void CallBack();		// called when more data can be written
};

#endif // SYNCHCONSOLE_H
