// hash.h
//      Data structures to manage a hash table to relate arbitrary
//	keys to arbitrary values. A hash table allows efficient lookup
//	for the value given the key.
//
//	I've only tested this implementation when both the key and the
//	value are primitive types (ints or pointers).  There is no 
//	guarantee that it will work in general.  In particular, it
//	assumes that the "==" operator works for both keys and values.
//
//	In addition, the key must have Hash() defined:
//		unsigned Hash(Key k);
//			returns a randomized # based on value of key
//
//	The value must have a function defined to retrieve the key:
//		Key GetKey(T x);
//
//	The hash table automatically resizes itself as items are
//	put into the table.  The implementation uses chaining
//	to resolve hash conflicts.
//
//	Allocation and deallocation of the items in the table are to 
//	be done by the caller.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef HASH_H
#define HASH_H

#include "copyright.h"
#include "list.h"

// The following class defines a "hash table" -- allowing quick
// lookup according to the hash function defined for the items
// being put into the table.

template <class Key,class T> class HashIterator;

template <class Key, class T> 
class HashTable {
  public:
    HashTable(Key (*get)(T x), unsigned (*hFunc)(Key x));	
    				// initialize a hash table
    ~HashTable();		// deallocate a hash table

    void Insert(T item);	// Put item into hash table
    T Remove(Key key);		// Remove item from hash table.

    bool Find(Key key, T *itemPtr) const; 
    				// Find an item from its key
    bool IsInTable(Key key) { T dummy; return Find(key, &dummy); } 	
				// Is the item in the table?

    bool IsEmpty() { return numItems == 0; }	
				// does the table have anything in it

    void Apply(void (*f)(T)) const;
    				// apply function to all elements in table

    void SanityCheck() const;// is this still a legal hash table?
    void SelfTest(T *p, int numItems);	
    				// is the module working?

  private:
typedef List<T> *Bucket;

    Bucket *buckets;		// the array of hash buckets
    int numBuckets;		// the number of buckets
    int numItems;		// the number of items in the table
    
    Key (*getKey)(T x);		// get Key from value
    unsigned (*hash)(Key x);	// the hash function

    void InitBuckets(int size);// initialize bucket array
    void DeleteBuckets(Bucket *table, int size);
    				// deallocate bucket array
				
    int HashValue(Key key) const;
    				// which bucket does the key hash to?

    void ReHash();		// expand the hash table
    
    bool FindInBucket(int bucket, Key key, T *itemPtr) const; 
    				// find item in bucket
    int FindNextFullBucket(int start) const;
    				// find next full bucket starting from this one

    friend class HashIterator<Key,T>;
};

// The following class can be used to step through a hash table --
// same interface as ListIterator.  Example code:
//	HashIterator<Key, T> iter(table); 
//
//	for (; !iter->IsDone(); iter->Next()) {
//	    Operation on iter->Item()
//      }

template <class Key,class T>
class HashIterator {
  public:
    HashIterator(HashTable<Key,T> *table); // initialize an iterator
    ~HashIterator() { if (bucketIter != NULL) delete bucketIter;}; 
				// destruct an iterator

    bool IsDone() { return (bucket == table->numBuckets); };
				// return TRUE if no more items in table 
    T Item() { ASSERT(!IsDone()); return bucketIter->Item(); }; 
				// return current item in table
    void Next(); 		// update iterator to point to next

  private:   
    HashTable<Key,T> *table;	// the hash table we're stepping through
    int bucket;			// current bucket we are in
    ListIterator<T> *bucketIter; // where we are in the bucket
};

#include "hash.cc"		// templates are really like macros
				// so needs to be included in every
				// file that uses the template
#endif // HASH_H
