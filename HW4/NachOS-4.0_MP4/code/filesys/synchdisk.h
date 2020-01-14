// synchdisk.h 
// 	Data structures to export a synchronous interface to the raw 
//	disk device.

#include "copyright.h"

#ifndef SYNCHDISK_H
#define SYNCHDISK_H

#include "disk.h"
#include "synch.h"
#include "callback.h"

// The following class defines a "synchronous" disk abstraction.
// As with other I/O devices, the raw physical disk is an asynchronous device --
// requests to read or write portions of the disk return immediately,
// and an interrupt occurs later to signal that the operation completed.
// (Also, the physical characteristics of the disk device assume that
// only one operation can be requested at a time).
//
// **This class provides the abstraction that for any individual thread
// making a request, it waits around until the operation finishes before
// returning.**

class SynchDisk : public CallBackObj {
  public:
    SynchDisk();   // Initialize a synchronous disk, by initializing the raw Disk.
    ~SynchDisk();  // De-allocate the synch disk data
    
    // Read/write a disk sector, returning only once the data is actually read or written.  
    // These call Disk::ReadRequest/WriteRequest and then wait until the request is done.
    void ReadSector(int sectorNumber, char* data);					
    void WriteSector(int sectorNumber, char* data);
    
    // Called by the disk device interrupt handler, to signal that the current disk operation is complete.
    void CallBack();

  private:
    Disk *disk;		  		  // Raw disk device
    Semaphore *semaphore; // To synchronize requesting thread with the interrupt handler
    Lock *lock;		  		  // Only one read/write request can be sent to the disk at a time
};

#endif // SYNCHDISK_H
