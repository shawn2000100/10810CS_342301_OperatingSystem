// pbitmap.c 
//	Routines to manage a persistent bitmap -- a bitmap that is
//	stored on disk.
//
// Copyright (c) 1992,1993,1995 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "pbitmap.h"

//----------------------------------------------------------------------
// PersistentBitmap::PersistentBitmap(int)
// 	Initialize a bitmap with "numItems" bits, so that every bit is clear.
//	it can be added somewhere on a list.
//
//	"numItems" is the number of bits in the bitmap.
//
//      This constructor does not initialize the bitmap from a disk file
//----------------------------------------------------------------------

PersistentBitmap::PersistentBitmap(int numItems):Bitmap(numItems) 
{ 
}

//----------------------------------------------------------------------
// PersistentBitmap::PersistentBitmap(OpenFile*,int)
// 	Initialize a persistent bitmap with "numItems" bits,
//      so that every bit is clear.
//
//	"numItems" is the number of bits in the bitmap.
//      "file" refers to an open file containing the bitmap (written
//        by a previous call to PersistentBitmap::WriteBack
//
//      This constructor initializes the bitmap from a disk file
//----------------------------------------------------------------------

PersistentBitmap::PersistentBitmap(OpenFile *file, int numItems):Bitmap(numItems) 
{ 
    // map has already been initialized by the BitMap constructor,
    // but we will just overwrite that with the contents of the
    // map found in the file
    file->ReadAt((char *)map, numWords * sizeof(unsigned), 0);
}

//----------------------------------------------------------------------
// PersistentBitmap::~PersistentBitmap
// 	De-allocate a persistent bitmap.
//----------------------------------------------------------------------

PersistentBitmap::~PersistentBitmap()
{ 
}

//----------------------------------------------------------------------
// PersistentBitmap::FetchFrom
// 	Initialize the contents of a persistent bitmap from a Nachos file.
//
//	"file" is the place to read the bitmap from
//----------------------------------------------------------------------

void
PersistentBitmap::FetchFrom(OpenFile *file) 
{
    file->ReadAt((char *)map, numWords * sizeof(unsigned), 0);
}

//----------------------------------------------------------------------
// PersistentBitmap::WriteBack
// 	Store the contents of a persistent bitmap to a Nachos file.
//
//	"file" is the place to write the bitmap to
//----------------------------------------------------------------------

void
PersistentBitmap::WriteBack(OpenFile *file)
{
   file->WriteAt((char *)map, numWords * sizeof(unsigned), 0);
}
