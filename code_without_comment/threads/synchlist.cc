// synchlist.cc
//	Routines for synchronized access to a list.
//
// 	Implemented in "monitor"-style -- surround each procedure with a
// 	lock acquire and release pair, using condition signal and wait for
// 	synchronization.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchlist.h"

//----------------------------------------------------------------------
// SynchList<T>::SynchList
//	Allocate and initialize the data structures needed for a 
//	synchronized list, empty to start with.
//	Elements can now be added to the list.
//----------------------------------------------------------------------

template <class T>
SynchList<T>::SynchList()
{
    list = new List<T>;
    lock = new Lock("list lock"); 
    listEmpty = new Condition("list empty cond");
}

//----------------------------------------------------------------------
// SynchList<T>::~SynchList
//	De-allocate the data structures created for synchronizing a list. 
//----------------------------------------------------------------------

template <class T>
SynchList<T>::~SynchList()
{ 
    delete listEmpty;
    delete lock;
    delete list;
}

//----------------------------------------------------------------------
// SynchList<T>::Append
//      Append an "item" to the end of the list.  Wake up anyone
//	waiting for an element to be appended.
//
//	"item" is the thing to put on the list. 
//----------------------------------------------------------------------

template <class T>
void
SynchList<T>::Append(T item)
{
    lock->Acquire();		// enforce mutual exclusive access to the list 
    list->Append(item);
    listEmpty->Signal(lock);	// wake up a waiter, if any
    lock->Release();
}

//----------------------------------------------------------------------
// SynchList<T>::RemoveFront
//      Remove an "item" from the beginning of the list.  Wait if
//	the list is empty.
// Returns:
//	The removed item. 
//----------------------------------------------------------------------

template <class T>
T
SynchList<T>::RemoveFront()
{
    T item;

    lock->Acquire();			// enforce mutual exclusion
    while (list->IsEmpty())
	listEmpty->Wait(lock);		// wait until list isn't empty
    item = list->RemoveFront();
    lock->Release();
    return item;
}

//----------------------------------------------------------------------
// SynchList<T>::Apply
//      Apply function to every item on a list.
//
//      "func" -- the function to apply
//----------------------------------------------------------------------

template <class T>
void
SynchList<T>::Apply(void (*func)(T))
{
    lock->Acquire();			// enforce mutual exclusion
    list->Apply(func);
    lock->Release();
}

//----------------------------------------------------------------------
// SynchList<T>::SelfTest, SelfTestHelper
//	Test whether the SynchList implementation is working,
//	by having two threads ping-pong a value between them
//	using two synchronized lists.
//----------------------------------------------------------------------

template <class T>
void
SynchList<T>::SelfTestHelper (void* data) 
{
    SynchList<T>* _this = (SynchList<T>*)data;
    for (int i = 0; i < 10; i++) {
        _this->Append(_this->selfTestPing->RemoveFront());
    }
}

template <class T>
void
SynchList<T>::SelfTest(T val)
{
    Thread *helper = new Thread("ping", 1);
    
    ASSERT(list->IsEmpty());
    selfTestPing = new SynchList<T>;
    helper->Fork(SynchList<T>::SelfTestHelper, this);
    for (int i = 0; i < 10; i++) {
        selfTestPing->Append(val);
	ASSERT(val == this->RemoveFront());
    }
    delete selfTestPing;
}
