// pbitmap.h 
//	Data structures defining a "persistent" bitmap -- a bitmap
//	that can be stored and fetched off of disk
//
//    A persistent bitmap can either be initialized from the disk
//    when it is created, or it can be initialized later using
//    the FetchFrom method
//
// Copyright (c) 1992,1993,1995 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef PBITMAP_H
#define PBITMAP_H

#include "copyright.h"
#include "bitmap.h"
#include "openfile.h"

// The following class defines a persistent bitmap.  It inherits all
// the behavior of a bitmap (see bitmap.h), adding the ability to
// be read from and stored to the disk.

class PersistentBitmap : public Bitmap {
  public:
    PersistentBitmap(OpenFile *file,int numItems); //initialize bitmap from disk 
    PersistentBitmap(int numItems); // or don't...

    ~PersistentBitmap(); 			// deallocate bitmap

    void FetchFrom(OpenFile *file);     // read bitmap from the disk
    void WriteBack(OpenFile *file); 	// write bitmap contents to disk 
};

#endif // PBITMAP_H
