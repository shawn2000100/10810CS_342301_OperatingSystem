// list.cc 
//     	Routines to manage a singly linked list of "things".
//	Lists are implemented as templates so that we can store
//	anything on the list in a type-safe manner.
//
// 	A "ListElement" is allocated for each item to be put on the
//	list; it is de-allocated when the item is removed. This means
//      we don't need to keep a "next" pointer in every object we
//      want to put on a list.
// 
//     	NOTE: Mutual exclusion must be provided by the caller.
//  	If you want a synchronized list, you must use the routines 
//	in synchlist.cc.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

//----------------------------------------------------------------------
// ListElement<T>::ListElement
// 	Initialize a list element, so it can be added somewhere on a list.
//
//	"itm" is the thing to be put on the list.
//----------------------------------------------------------------------

template <class T>
ListElement<T>::ListElement(T itm)
{
     item = itm;
     next = NULL;	// always initialize to something!
}


//----------------------------------------------------------------------
// List<T>::List
//	Initialize a list, empty to start with.
//	Elements can now be added to the list.
//----------------------------------------------------------------------

template <class T>
List<T>::List()
{ 
    first = last = NULL; 
    numInList = 0;
}

//----------------------------------------------------------------------
// List<T>::~List
//	Prepare a list for deallocation.  
//      This does *NOT* free list elements, nor does it
//      free the data those elements point to.
//      Normally, the list should be empty when this is called.
//----------------------------------------------------------------------

template <class T>
List<T>::~List()
{ 
}

//----------------------------------------------------------------------
// List<T>::Append
//      Append an "item" to the end of the list.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, put it at the end.
//
//	"item" is the thing to put on the list.
//----------------------------------------------------------------------

template <class T>
void
List<T>::Append(T item)
{
    ListElement<T> *element = new ListElement<T>(item);

    ASSERT(!IsInList(item));
    if (IsEmpty()) {		// list is empty
	first = element;
	last = element;
    } else {			// else put it after last
	last->next = element;
	last = element;
    }
    numInList++;
    ASSERT(IsInList(item));
}

//----------------------------------------------------------------------
// List<T>::Prepend
//	Same as Append, only put "item" on the front.
//----------------------------------------------------------------------

template <class T>
void
List<T>::Prepend(T item)
{
    ListElement<T> *element = new ListElement<T>(item);

    ASSERT(!IsInList(item));
    if (IsEmpty()) {		// list is empty
	first = element;
	last = element;
    } else {			// else put it before first
	element->next = first;
	first = element;
    }
    numInList++;
    ASSERT(IsInList(item));
}

//----------------------------------------------------------------------
// List<T>::RemoveFront
//      Remove the first "item" from the front of the list.
//	List must not be empty.
// 
// Returns:
//	The removed item.
//----------------------------------------------------------------------

template <class T>
T
List<T>::RemoveFront()
{
    ListElement<T> *element = first;
    T thing;

    ASSERT(!IsEmpty());

    thing = first->item;
    if (first == last) {	// list had one item, now has none 
        first = NULL;
		last = NULL;
    } else {
        first = element->next;
    }
    numInList--;
    delete element;
    return thing;
}

//----------------------------------------------------------------------
// List<T>::Remove
//      Remove a specific item from the list.  Must be in the list!
//----------------------------------------------------------------------

template <class T>
void
List<T>::Remove(T item)
{
    ListElement<T> *prev, *ptr;
    T removed;

    ASSERT(IsInList(item));

    // if first item on list is match, then remove from front
    if (item == first->item) {	
        removed = RemoveFront();
        ASSERT(item == removed);
    } else {
	prev = first;
        for (ptr = first->next; ptr != NULL; prev = ptr, ptr = ptr->next) {
            if (item == ptr->item) {
		prev->next = ptr->next;
		if (prev->next == NULL) {
		    last = prev;
		}
		delete ptr;
		numInList--;
		break;
	    }
        }
	ASSERT(ptr != NULL);	// should always find item!
    }
   ASSERT(!IsInList(item));
}

//----------------------------------------------------------------------
// List<T>::IsInList
//      Return TRUE if the item is in the list.
//----------------------------------------------------------------------

template <class T>
bool
List<T>::IsInList(T item) const
{ 
    ListElement<T> *ptr;

    for (ptr = first; ptr != NULL; ptr = ptr->next) {
        if (item == ptr->item) {
            return TRUE;
        }
    }
    return FALSE;
}


//----------------------------------------------------------------------
// List<T>::Apply
//      Apply function to every item on a list.
//
//	"func" -- the function to apply
//----------------------------------------------------------------------

template <class T>
void
List<T>::Apply(void (*func)(T)) const
{ 
    ListElement<T> *ptr;

    for (ptr = first; ptr != NULL; ptr = ptr->next) {
        (*func)(ptr->item);
    }
}


//----------------------------------------------------------------------
// SortedList::Insert
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//
//	"item" is the thing to put on the list. 
//----------------------------------------------------------------------

template <class T>
void
SortedList<T>::Insert(T item)
{
    ListElement<T> *element = new ListElement<T>(item);
    ListElement<T> *ptr;		// keep track

    ASSERT(!IsInList(item));
    if (this->IsEmpty()) {			// if list is empty, put at front
        this->first = element;
        this->last = element;
    } else if (compare(item, this->first->item) < 0) {  // item goes at front 
	element->next = this->first;
	this->first = element;
    } else {		// look for first elt in list bigger than item
        for (ptr = this->first; ptr->next != NULL; ptr = ptr->next) {
            if (compare(item, ptr->next->item) < 0) {
		element->next = ptr->next;
	        ptr->next = element;
		this->numInList++;
		return;
	    }
	}
	this->last->next = element;		// item goes at end of list
	this->last = element;
    }
    this->numInList++;
    ASSERT(IsInList(item));
}

//----------------------------------------------------------------------
// List::SanityCheck
//      Test whether this is still a legal list.
//
//	Tests: do I get to last starting from first?
//	       does the list have the right # of elements?
//----------------------------------------------------------------------

template <class T>
void 
List<T>::SanityCheck() const
{
    ListElement<T> *ptr;
    int numFound;

    if (first == NULL) {
	ASSERT((numInList == 0) && (last == NULL));
    } else if (first == last) {
	ASSERT((numInList == 1) && (last->next == NULL));
    } else {
        for (numFound = 1, ptr = first; ptr != last; ptr = ptr->next) {
	    numFound++;
            ASSERT(numFound <= numInList);	// prevent infinite loop
        }
        ASSERT(numFound == numInList);
        ASSERT(last->next == NULL);
    }
}

//----------------------------------------------------------------------
// List::SelfTest
//      Test whether this module is working.
//----------------------------------------------------------------------

template <class T>
void 
List<T>::SelfTest(T *p, int numEntries)
{
    int i;
    ListIterator<T> *iterator = new ListIterator<T>(this);

    SanityCheck();
    // check various ways that list is empty
    ASSERT(IsEmpty() && (first == NULL));
    for (; !iterator->IsDone(); iterator->Next()) {
	ASSERTNOTREACHED();	// nothing on list
    }

    for (i = 0; i < numEntries; i++) {
	 Append(p[i]);
	 ASSERT(IsInList(p[i]));
	 ASSERT(!IsEmpty());
     }
     SanityCheck();

     // should be able to get out everything we put in
     for (i = 0; i < numEntries; i++) {
	 Remove(p[i]);
         ASSERT(!IsInList(p[i]));
     }
     ASSERT(IsEmpty());
     SanityCheck();
     delete iterator;
}

//----------------------------------------------------------------------
// SortedList::SanityCheck
//      Test whether this is still a legal sorted list.
//
//	Test: is the list sorted?
//----------------------------------------------------------------------

template <class T>
void 
SortedList<T>::SanityCheck() const
{
    ListElement<T> *prev, *ptr;

    List<T>::SanityCheck();
    if (this->first != this->last) {
        for (prev = this->first, ptr = this->first->next; ptr != NULL; 
						prev = ptr, ptr = ptr->next) {
            ASSERT(compare(prev->item, ptr->item) <= 0);
        }
    }
}

//----------------------------------------------------------------------
// SortedList::SelfTest
//      Test whether this module is working.
//----------------------------------------------------------------------

template <class T>
void 
SortedList<T>::SelfTest(T *p, int numEntries)
{
    int i;
    T *q = new T[numEntries];

    List<T>::SelfTest(p, numEntries);

    for (i = 0; i < numEntries; i++) {
	 Insert(p[i]);
	 ASSERT(IsInList(p[i]));
     }
     SanityCheck();

     // should be able to get out everything we put in
     for (i = 0; i < numEntries; i++) {
	 q[i] = this->RemoveFront();
         ASSERT(!IsInList(q[i]));
     }
     ASSERT(this->IsEmpty());

     // make sure everything came out in the right order
     for (i = 0; i < (numEntries - 1); i++) {
	 ASSERT(compare(q[i], q[i + 1]) <= 0);
     }
     SanityCheck();

     delete q;
}
