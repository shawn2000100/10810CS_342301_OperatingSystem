// libtest.cc 
//	Driver code to call self-test routines for standard library
//	classes -- bitmaps, lists, sorted lists, and hash tables.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "libtest.h"
#include "bitmap.h"
#include "list.h"
#include "hash.h"
#include "sysdep.h"

//----------------------------------------------------------------------
// IntCompare
//	Compare two integers together.  Serves as the comparison
//	function for testing SortedLists
//----------------------------------------------------------------------

static int 
IntCompare(int x, int y) {
    if (x < y) return -1;
    else if (x == y) return 0;
    else return 1;
}

//----------------------------------------------------------------------
// HashInt, HashKey
//	Compute a hash function on an integer.  Serves as the
//	hashing function for testing HashTables.
//----------------------------------------------------------------------

static unsigned int 
HashInt(int key) {
    return (unsigned int) key;
}

//----------------------------------------------------------------------
// HashKey
//	Convert a string into an integer.  Serves as the function
//	to retrieve the key from the item in the hash table, for
//	testing HashTables.  Should be able to use "atoi" directly,
//	but some compilers complain about that.
//----------------------------------------------------------------------

static int 
HashKey(char *str) {
    return atoi(str);
}

// Array of values to be inserted into a List or SortedList. 
static int listTestVector[] = { 9, 5, 7 };

// Array of values to be inserted into the HashTable
// There are enough here to force a ReHash().
static char *hashTestVector[] = { "0", "1", "2", "3", "4", "5", "6",
	 "7", "8", "9", "10", "11", "12", "13", "14"};

//----------------------------------------------------------------------
// LibSelfTest
//	Run self tests on bitmaps, lists, sorted lists, and 
//	hash tables.
//----------------------------------------------------------------------

void
LibSelfTest () {
    Bitmap *map = new Bitmap(200);
    List<int> *list = new List<int>;
    SortedList<int> *sortList = new SortedList<int>(IntCompare);
    HashTable<int, char *> *hashTable = 
	new HashTable<int, char *>(HashKey, HashInt);
	
		
    map->SelfTest();
    list->SelfTest(listTestVector, sizeof(listTestVector)/sizeof(int));
    sortList->SelfTest(listTestVector, sizeof(listTestVector)/sizeof(int));
    hashTable->SelfTest(hashTestVector, sizeof(hashTestVector)/sizeof(char *));

    delete map;
    delete list;
    delete sortList;
    delete hashTable;
}
