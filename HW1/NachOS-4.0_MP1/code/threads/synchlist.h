// synchlist.h 
//	Data structures for synchronized access to a list.
//
//	Identical interface to List, except accesses are synchronized.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYNCHLIST_H
#define SYNCHLIST_H

#include "copyright.h"
#include "list.h"
#include "synch.h"

// The following class defines a "synchronized list" -- a list for which
// these constraints hold:
//	1. Threads trying to remove an item from a list will
//	wait until the list has an element on it.
//	2. One thread at a time can access list data structures

template <class T>
class SynchList {
  public:
    SynchList();		// initialize a synchronized list
    ~SynchList();		// de-allocate a synchronized list

    void Append(T item);	// append item to the end of the list,
				// and wake up any thread waiting in remove

    T RemoveFront();		// remove the first item from the front of
				// the list, waiting if the list is empty

    void Apply(void (*f)(T)); // apply function to all elements in list

    void SelfTest(T value);	// test the SynchList implementation
    
  private:
    List<T> *list;		// the list of things
    Lock *lock;			// enforce mutual exclusive access to the list
    Condition *listEmpty;	// wait in Remove if the list is empty
    
    // these are only to assist SelfTest()
    SynchList<T> *selfTestPing;
    static void SelfTestHelper(void* data);
};

#include "synchlist.cc"

#endif // SYNCHLIST_H
