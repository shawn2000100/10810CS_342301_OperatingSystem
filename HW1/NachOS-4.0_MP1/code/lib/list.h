// list.h 
//	Data structures to manage LISP-like lists.  
//
//      As in LISP, a list can contain any type of data structure
//	as an item on the list: thread control blocks, 
//	pending interrupts, etc.  Allocation and deallocation of the
//	items on the list are to be done by the caller.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef LIST_H
#define LIST_H

#include "copyright.h"
#include "debug.h"

// The following class defines a "list element" -- which is
// used to keep track of one item on a list.  It is equivalent to a
// LISP cell, with a "car" ("next") pointing to the next element on the list,
// and a "cdr" ("item") pointing to the item on the list.
//
// This class is private to this module (and classes that inherit
// from this module). Made public for notational convenience.

template <class T>
class ListElement {
  public:
    ListElement(T itm); 	// initialize a list element
    ListElement *next;	     	// next element on list, NULL if this is last
    T item; 	   	     	// item on the list
};

// The following class defines a "list" -- a singly linked list of
// list elements, each of which points to a single item on the list.
// The class has been tested only for primitive types (ints, pointers);
// no guarantees it will work in general.  For instance, all types
// to be inserted into a list must have a "==" operator defined.

template <class T> class ListIterator;

template <class T>
class List {
  public:
    List();			// initialize the list
    virtual ~List();		// de-allocate the list

    virtual void Prepend(T item);// Put item at the beginning of the list
    virtual void Append(T item); // Put item at the end of the list

    T Front() { return first->item; }
    				// Return first item on list
				// without removing it
    T RemoveFront(); 		// Take item off the front of the list
    void Remove(T item); 	// Remove specific item from list

    bool IsInList(T item) const;// is the item in the list?

    unsigned int NumInList() { return numInList;};
    				// how many items in the list?
    bool IsEmpty() { return (numInList == 0); };
    				// is the list empty? 

    void Apply(void (*f)(T)) const; 
    				// apply function to all elements in list

    virtual void SanityCheck() const;	
				// has this list been corrupted?
    void SelfTest(T *p, int numEntries);
				// verify module is working

  protected:
    ListElement<T> *first;  	// Head of the list, NULL if list is empty
    ListElement<T> *last;	// Last element of list
    int numInList;		// number of elements in list

    friend class ListIterator<T>;
};

// The following class defines a "sorted list" -- a singly linked list of
// list elements, arranged so that "Remove" always returns the smallest 
// element. 
// All types to be inserted onto a sorted list must have a "Compare"
// function defined:
//	   int Compare(T x, T y) 
//		returns -1 if x < y
//		returns 0 if x == y
//		returns 1 if x > y

template <class T>
class SortedList : public List<T> {
  public:
    SortedList(int (*comp)(T x, T y)) : List<T>() { compare = comp;};
    ~SortedList() {};		// base class destructor called automatically

    void Insert(T item); 	// insert an item onto the list in sorted order

    void SanityCheck() const;	// has this list been corrupted?
    void SelfTest(T *p, int numEntries);
				// verify module is working

  private:
    int (*compare)(T x, T y);	// function for sorting list elements

    void Prepend(T item) { Insert(item); }  // *pre*pending has no meaning 
				             //	in a sorted list
    void Append(T item) { Insert(item); }   // neither does *ap*pend 

};

// The following class can be used to step through a list. 
// Example code:
//	ListIterator<T> *iter(list); 
//
//	for (; !iter->IsDone(); iter->Next()) {
//	    Operation on iter->Item()
//      }

template <class T>
class ListIterator {
  public:
    ListIterator(List<T> *list) { current = list->first; } 
				// initialize an iterator

    bool IsDone() { return current == NULL; };
				// return TRUE if we are at the end of the list

    T Item() { ASSERT(!IsDone()); return current->item; };
				// return current element on list

    void Next() { current = current->next; };		
				// update iterator to point to next

  private:
    ListElement<T> *current;	// where we are in the list
};

#include "list.cc"		// templates are really like macros
				// so needs to be included in every
				// file that uses the template
#endif // LIST_H
