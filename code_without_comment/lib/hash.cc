// hash.cc 
//     	Routines to manage a self-expanding hash table of arbitrary things.
//	The hashing function is supplied by the objects being put into
//	the table; we use chaining to resolve hash conflicts.
//
//	The hash table is implemented as an array of sorted lists,
//	and we expand the hash table if the number of elements in the table
//	gets too big.
// 
//     	NOTE: Mutual exclusion must be provided by the caller.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

const int InitialBuckets = 4;	// how big a hash table do we start with
const int ResizeRatio = 3;	// when do we grow the hash table?
const int IncreaseSizeBy = 4;	// how much do we grow table when needed?

#include "copyright.h"

//----------------------------------------------------------------------
// HashTable<Key,T>::HashTable
//	Initialize a hash table, empty to start with.
//	Elements can now be added to the table.
//----------------------------------------------------------------------

template <class Key, class T>
HashTable<Key,T>::HashTable(Key (*get)(T x), unsigned (*hFunc)(Key x))
{ 
    numItems = 0;
    InitBuckets(InitialBuckets);
    getKey = get;
    hash = hFunc;
}

//----------------------------------------------------------------------
// HashTable<Key,T>::InitBuckets
//	Initialize the bucket array for a hash table.
//	Called by the constructor and by ReHash().
//----------------------------------------------------------------------

template <class Key, class T>
void
HashTable<Key,T>::InitBuckets(int sz)
{ 
    numBuckets = sz;
    buckets = new Bucket[numBuckets];
    for (int i = 0; i < sz; i++) {
    	buckets[i] = new List<T>;
    }
}

//----------------------------------------------------------------------
// HashTable<T>::~HashTable
//	Prepare a hash table for deallocation.  
//----------------------------------------------------------------------

template <class Key, class T>
HashTable<Key,T>::~HashTable()
{ 
    ASSERT(IsEmpty());		// make sure table is empty
    DeleteBuckets(buckets, numBuckets);
}

//----------------------------------------------------------------------
// HashTable<Key,T>::DeleteBuckets
//	De-Initialize the bucket array for a hash table.
//	Called by the destructor and by ReHash().
//----------------------------------------------------------------------

template <class Key, class T>
void
HashTable<Key,T>::DeleteBuckets(List<T> **table, int sz)
{ 
    for (int i = 0; i < sz; i++) {
    	delete table[i];
    }
    delete [] table;
}

//----------------------------------------------------------------------
// HashTable<Key,T>::HashValue
//      Return hash table bucket that would contain key.
//----------------------------------------------------------------------

template <class Key, class T>
int
HashTable<Key, T>::HashValue(Key key) const 
{
    int result = (*hash)(key) % numBuckets;
    ASSERT(result >= 0 && result < numBuckets);
    return result;
}

//----------------------------------------------------------------------
// HashTable<Key,T>::Insert
//      Put an item into the hashtable.
//      
//	Resize the table if the # of elements / # of buckets is too big.
//	Then allocate a HashElement to keep track of the key, item pair,
//	and add it to the right bucket.
//
//	"key" is the key we'll use to find this item.
//	"item" is the thing to put in the table.
//----------------------------------------------------------------------

template <class Key, class T>
void
HashTable<Key,T>::Insert(T item)
{
    Key key = getKey(item);

    ASSERT(!IsInTable(key));

    if ((numItems / numBuckets) >= ResizeRatio) {
	ReHash();
    }

    buckets[HashValue(key)]->Append(item);
    numItems++;

    ASSERT(IsInTable(key));
}

//----------------------------------------------------------------------
// HashTable<Key,T>::ReHash
//      Increase the size of the hashtable, by 
//	  (i) making a new table
//	  (ii) moving all the elements into the new table
//	  (iii) deleting the old table
//----------------------------------------------------------------------

template <class Key, class T>
void
HashTable<Key,T>::ReHash()
{
    Bucket *oldTable = buckets;
    int oldSize = numBuckets;
    T item;

    SanityCheck();
    InitBuckets(numBuckets * IncreaseSizeBy);

    for (int i = 0; i < oldSize; i++) {
	while (!oldTable[i]->IsEmpty()) {
	    item = oldTable[i]->RemoveFront();
	    buckets[HashValue(getKey(item))]->Append(item);
        }
    }
    DeleteBuckets(oldTable, oldSize);
    SanityCheck();
}

//----------------------------------------------------------------------
// HashTable<Key,T>::FindInBucket
//      Find an item in a hash table bucket, from it's key
//
//	"bucket" -- the list storing the item, if it's in the table 
//	"key" -- the key uniquely identifying the item
// 
// Returns:
//	Whether item is found, and if found, the item.
//----------------------------------------------------------------------

template <class Key, class T>
bool
HashTable<Key,T>::FindInBucket(int bucket, 
				Key key, T *itemPtr) const
{
    ListIterator<T> iterator(buckets[bucket]);

    for (; !iterator.IsDone(); iterator.Next()) {
	if (key == getKey(iterator.Item())) { // found!
	    *itemPtr = iterator.Item();
	    return TRUE;
        }
    }
    *itemPtr = NULL;
    return FALSE;
}

//----------------------------------------------------------------------
// HashTable<Key,T>::Find
//      Find an item from the hash table.
// 
// Returns:
//	The item or NULL if not found. 
//----------------------------------------------------------------------

template <class Key, class T>
bool
HashTable<Key,T>::Find(Key key, T *itemPtr) const
{
    int bucket = HashValue(key);
    
    return FindInBucket(bucket, key, itemPtr); 
}

//----------------------------------------------------------------------
// HashTable<Key,T>::Remove
//      Remove an item from the hash table. The item must be in the table.
// 
// Returns:
//	The removed item.
//----------------------------------------------------------------------

template <class Key, class T>
T
HashTable<Key,T>::Remove(Key key)
{
    int bucket = HashValue(key);
    T item;
    bool found = FindInBucket(bucket, key, &item); 

    ASSERT(found);	// item must be in table

    buckets[bucket]->Remove(item);
    numItems--;

    ASSERT(!IsInTable(key));
    return item;
}


//----------------------------------------------------------------------
// HashTable<Key,T>::Apply
//      Apply function to every item in the hash table.
//
//	"func" -- the function to apply
//----------------------------------------------------------------------

template <class Key,class T>
void
HashTable<Key,T>::Apply(void (*func)(T)) const
{
    for (int bucket = 0; bucket < numBuckets; bucket++) {
        buckets[bucket]->Apply(func);
    }
}

//----------------------------------------------------------------------
// HashTable<Key,T>::FindNextFullBucket
//      Find the next bucket in the hash table that has any items in it.
//
//	"bucket" -- where to start looking for full buckets
//----------------------------------------------------------------------

template <class Key,class T>
int
HashTable<Key,T>::FindNextFullBucket(int bucket) const
{ 
    for (; bucket < numBuckets; bucket++) {
	if (!buckets[bucket]->IsEmpty()) {
	     break;
	}
    }
    return bucket;
}

//----------------------------------------------------------------------
// HashTable<Key,T>::SanityCheck
//      Test whether this is still a legal hash table.
//
//	Tests: are all the buckets legal?
//	       does the table have the right # of elements?
//	       do all the elements hash to where they are stored?
//----------------------------------------------------------------------

template <class Key, class T>
void 
HashTable<Key,T>::SanityCheck() const
{
    int numFound = 0;
    ListIterator<T> *iterator;

    for (int i = 0; i < numBuckets; i++) {
	buckets[i]->SanityCheck();
	numFound += buckets[i]->NumInList();
	iterator = new ListIterator<T>(buckets[i]);
        for (; !iterator->IsDone(); iterator->Next()) {
	    ASSERT(i == HashValue(getKey(iterator->Item())));
        }
        delete iterator;
    }
    ASSERT(numItems == numFound);

}

//----------------------------------------------------------------------
// HashTable<Key,T>::SelfTest
//      Test whether this module is working.
//----------------------------------------------------------------------

template <class Key, class T>
void 
HashTable<Key,T>::SelfTest(T *p, int numEntries)
{
    int i;
    HashIterator<Key, T> *iterator = new HashIterator<Key,T>(this);
    
    SanityCheck();
    ASSERT(IsEmpty());	// check that table is empty in various ways
    for (; !iterator->IsDone(); iterator->Next()) {
	ASSERTNOTREACHED();
    }
    delete iterator;

    for (i = 0; i < numEntries; i++) {
        Insert(p[i]);
        ASSERT(IsInTable(getKey(p[i])));
        ASSERT(!IsEmpty());
    }
    
    // should be able to get out everything we put in
    for (i = 0; i < numEntries; i++) {  
        ASSERT(Remove(getKey(p[i])) == p[i]);
    }

    ASSERT(IsEmpty());
    SanityCheck();
}


//----------------------------------------------------------------------
// HashIterator<Key,T>::HashIterator
//      Initialize a data structure to allow us to step through
//	every entry in a has table.
//----------------------------------------------------------------------

template <class Key, class T>
HashIterator<Key,T>::HashIterator(HashTable<Key,T> *tbl) 
{ 
    table = tbl;
    bucket = table->FindNextFullBucket(0);
    bucketIter = NULL;
    if (bucket < table->numBuckets) {
	bucketIter = new ListIterator<T>(table->buckets[bucket]);
    }
}

//----------------------------------------------------------------------
// HashIterator<Key,T>::Next
//      Update iterator to point to the next item in the table.
//----------------------------------------------------------------------

template <class Key,class T>
void
HashIterator<Key,T>::Next() 
{ 
    bucketIter->Next();
    if (bucketIter->IsDone()) {
	delete bucketIter;
	bucketIter = NULL;
        bucket = table->FindNextFullBucket(++bucket);
        if (bucket < table->numBuckets) {
	    bucketIter = new ListIterator<T>(table->buckets[bucket]);
        }
    }
}
