// disk.cc 
//	Routines to simulate a physical disk device; reading and writing
//	to the disk is simulated as reading and writing to a UNIX file.
//	See disk.h for details about the behavior of disks (and
//	therefore about the behavior of this simulation).
//
//	Disk operations are asynchronous, so we have to invoke an interrupt
//	handler when the simulated operation completes.
//
//  DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "disk.h"
#include "debug.h"
#include "sysdep.h"
#include "main.h"

// We put a magic number at the front of the UNIX file representing the
// disk, to make it less likely we will accidentally treat a useful file 
// as a disk (which would probably trash the file's contents).

const int MagicNumber = 0x456789ab;
const int MagicSize = sizeof(int);
const int DiskSize = (MagicSize + (NumSectors * SectorSize));


//----------------------------------------------------------------------
// Disk::Disk()
// 	Initialize a simulated disk.  Open the UNIX file (creating it
//	if it doesn't exist), and check the magic number to make sure it's 
// 	ok to treat it as Nachos disk storage.
//
//	"toCall" -- object to call when disk read/write request completes
//----------------------------------------------------------------------

Disk::Disk(CallBackObj *toCall)
{
    int magicNum;
    int tmp = 0;

    DEBUG(dbgDisk, "Initializing the disk.");
    callWhenDone = toCall;
    lastSector = 0;
    bufferInit = 0;
    
    sprintf(diskname,"DISK_%d",kernel->hostName);
    fileno = OpenForReadWrite(diskname, FALSE);
    if (fileno >= 0) {		 	// file exists, check magic number 
	Read(fileno, (char *) &magicNum, MagicSize);
	ASSERT(magicNum == MagicNumber);
    } else {				// file doesn't exist, create it
        fileno = OpenForWrite(diskname);
	magicNum = MagicNumber;  
	WriteFile(fileno, (char *) &magicNum, MagicSize); // write magic number

	// need to write at end of file, so that reads will not return EOF
        Lseek(fileno, DiskSize - sizeof(int), 0);	
	WriteFile(fileno, (char *)&tmp, sizeof(int));  
    }
    active = FALSE;
}

//----------------------------------------------------------------------
// Disk::~Disk()
// 	Clean up disk simulation, by closing the UNIX file representing the
//	disk.
//----------------------------------------------------------------------

Disk::~Disk()
{
    Close(fileno);
}

//----------------------------------------------------------------------
// Disk::PrintSector()
// 	Dump the data in a disk read/write request, for debugging.
//----------------------------------------------------------------------

static void
PrintSector (bool writing, int sector, char *data)
{
    int *p = (int *) data;

    if (writing)
        cout << "Writing sector: " << sector << "\n"; 
    else
        cout << "Reading sector: " << sector << "\n"; 
    for (unsigned int i = 0; i < (SectorSize/sizeof(int)); i++) {
	cout << p[i] << " ";
    }
    cout << "\n"; 
}

//----------------------------------------------------------------------
// Disk::ReadRequest/WriteRequest
// 	Simulate a request to read/write a single disk sector
//	   Do the read/write immediately to the UNIX file
//	   Set up an interrupt handler to be called later,
//	      that will notify the caller when the simulator says
//	      the operation has completed.
//
//	Note that a disk only allows an entire sector to be read/written,
//	not part of a sector.
//
//	"sectorNumber" -- the disk sector to read/write
//	"data" -- the bytes to be written, the buffer to hold the incoming bytes
//----------------------------------------------------------------------

void
Disk::ReadRequest(int sectorNumber, char* data)
{
    int ticks = ComputeLatency(sectorNumber, FALSE);

    ASSERT(!active);				// only one request at a time
    ASSERT((sectorNumber >= 0) && (sectorNumber < NumSectors));
    
    DEBUG(dbgDisk, "Reading from sector " << sectorNumber);
    Lseek(fileno, SectorSize * sectorNumber + MagicSize, 0);
    Read(fileno, data, SectorSize);
    if (debug->IsEnabled('d'))
	PrintSector(FALSE, sectorNumber, data);
    
    active = TRUE;
    UpdateLast(sectorNumber);
    kernel->stats->numDiskReads++;
    kernel->interrupt->Schedule(this, ticks, DiskInt);
}

void
Disk::WriteRequest(int sectorNumber, char* data)
{
    int ticks = ComputeLatency(sectorNumber, TRUE);

    ASSERT(!active);
    ASSERT((sectorNumber >= 0) && (sectorNumber < NumSectors));
    
    DEBUG(dbgDisk, "Writing to sector " << sectorNumber);
    Lseek(fileno, SectorSize * sectorNumber + MagicSize, 0);
    WriteFile(fileno, data, SectorSize);
    if (debug->IsEnabled('d'))
	PrintSector(TRUE, sectorNumber, data);
    
    active = TRUE;
    UpdateLast(sectorNumber);
    kernel->stats->numDiskWrites++;
    kernel->interrupt->Schedule(this, ticks, DiskInt);
}

//----------------------------------------------------------------------
// Disk::CallBack()
// 	Called by the machine simulation when the disk interrupt occurs.
//----------------------------------------------------------------------

void
Disk::CallBack ()
{ 
    active = FALSE;
    callWhenDone->CallBack();
}

//----------------------------------------------------------------------
// Disk::TimeToSeek()
//	Returns how long it will take to position the disk head over the correct
//	track on the disk.  Since when we finish seeking, we are likely
//	to be in the middle of a sector that is rotating past the head,
//	we also return how long until the head is at the next sector boundary.
//	
//   	Disk seeks at one track per SeekTime ticks (cf. stats.h)
//   	and rotates at one sector per RotationTime ticks
//----------------------------------------------------------------------

int
Disk::TimeToSeek(int newSector, int *rotation) 
{
    int newTrack = newSector / SectorsPerTrack;
    int oldTrack = lastSector / SectorsPerTrack;
    int seek = abs(newTrack - oldTrack) * SeekTime;
				// how long will seek take?
    int over = (kernel->stats->totalTicks + seek) % RotationTime; 
				// will we be in the middle of a sector when
				// we finish the seek?

    *rotation = 0;
    if (over > 0)	 	// if so, need to round up to next full sector
   	*rotation = RotationTime - over;
    return seek;
}

//----------------------------------------------------------------------
// Disk::ModuloDiff()
// 	Return number of sectors of rotational delay between target sector
//	"to" and current sector position "from"
//----------------------------------------------------------------------

int 
Disk::ModuloDiff(int to, int from)
{
    int toOffset = to % SectorsPerTrack;
    int fromOffset = from % SectorsPerTrack;

    return ((toOffset - fromOffset) + SectorsPerTrack) % SectorsPerTrack;
}

//----------------------------------------------------------------------
// Disk::ComputeLatency()
// 	Return how long will it take to read/write a disk sector, from
//	the current position of the disk head.
//
//   	Latency = seek time + rotational latency + transfer time
//   	Disk seeks at one track per SeekTime ticks (cf. stats.h)
//   	and rotates at one sector per RotationTime ticks
//
//   	To find the rotational latency, we first must figure out where the 
//   	disk head will be after the seek (if any).  We then figure out
//   	how long it will take to rotate completely past newSector after 
//	that point.
//
//   	The disk also has a "track buffer"; the disk continuously reads
//   	the contents of the current disk track into the buffer.  This allows 
//   	read requests to the current track to be satisfied more quickly.
//   	The contents of the track buffer are discarded after every seek to 
//   	a new track.
//----------------------------------------------------------------------

int
Disk::ComputeLatency(int newSector, bool writing)
{
    int rotation;
    int seek = TimeToSeek(newSector, &rotation);
    int timeAfter = kernel->stats->totalTicks + seek + rotation;

#ifndef NOTRACKBUF	// turn this on if you don't want the track buffer stuff
    // check if track buffer applies
    if ((writing == FALSE) && (seek == 0) 
		&& (((timeAfter - bufferInit) / RotationTime) 
	     		> ModuloDiff(newSector, bufferInit / RotationTime))) {
        DEBUG(dbgDisk, "Request latency = " << RotationTime);
	return RotationTime; // time to transfer sector from the track buffer
    }
#endif

    rotation += ModuloDiff(newSector, timeAfter / RotationTime) * RotationTime;

    DEBUG(dbgDisk, "Request latency = " << (seek + rotation + RotationTime));
    return(seek + rotation + RotationTime);
}

//----------------------------------------------------------------------
// Disk::UpdateLast
//   	Keep track of the most recently requested sector.  So we can know
//	what is in the track buffer.
//----------------------------------------------------------------------

void
Disk::UpdateLast(int newSector)
{
    int rotate;
    int seek = TimeToSeek(newSector, &rotate);
    
    if (seek != 0)
	bufferInit = kernel->stats->totalTicks + seek + rotate;
    lastSector = newSector;
    DEBUG(dbgDisk, "Updating last sector = " << lastSector << " , " << bufferInit);
}
