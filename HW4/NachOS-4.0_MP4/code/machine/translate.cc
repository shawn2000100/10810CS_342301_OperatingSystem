// translate.cc 
//	Routines to translate virtual addresses to physical addresses.
//	Software sets up a table of legal translations.  We look up
//	in the table on every memory reference to find the true physical
//	memory location.
//
// Two types of translation are supported here.
//
//	Linear page table -- the virtual page # is used as an index
//	into the table, to find the physical page #.
//
//	Translation lookaside buffer -- associative lookup in the table
//	to find an entry with the same virtual page #.  If found,
//	this entry is used for the translation.
//	If not, it traps to software with an exception. 
//
//	In practice, the TLB is much smaller than the amount of physical
//	memory (16 entries is common on a machine that has 1000's of
//	pages).  Thus, there must also be a backup translation scheme
//	(such as page tables), but the hardware doesn't need to know
//	anything at all about that.
//
//	Note that the contents of the TLB are specific to an address space.
//	If the address space changes, so does the contents of the TLB!
//
// DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"

// Routines for converting Words and Short Words to and from the
// simulated machine's format of little endian.  These end up
// being NOPs when the host machine is also little endian (DEC and Intel).

unsigned int
WordToHost(unsigned int word) {
#ifdef HOST_IS_BIG_ENDIAN
	 register unsigned long result;
	 result = (word >> 24) & 0x000000ff;
	 result |= (word >> 8) & 0x0000ff00;
	 result |= (word << 8) & 0x00ff0000;
	 result |= (word << 24) & 0xff000000;
	 return result;
#else 
	 return word;
#endif /* HOST_IS_BIG_ENDIAN */
}

unsigned short
ShortToHost(unsigned short shortword) {
#ifdef HOST_IS_BIG_ENDIAN
	 register unsigned short result;
	 result = (shortword << 8) & 0xff00;
	 result |= (shortword >> 8) & 0x00ff;
	 return result;
#else 
	 return shortword;
#endif /* HOST_IS_BIG_ENDIAN */
}

unsigned int
WordToMachine(unsigned int word) { return WordToHost(word); }

unsigned short
ShortToMachine(unsigned short shortword) { return ShortToHost(shortword); }


//----------------------------------------------------------------------
// Machine::ReadMem
//      Read "size" (1, 2, or 4) bytes of virtual memory at "addr" into 
//	the location pointed to by "value".
//
//   	Returns FALSE if the translation step from virtual to physical memory
//   	failed.
//
//	"addr" -- the virtual address to read from
//	"size" -- the number of bytes to read (1, 2, or 4)
//	"value" -- the place to write the result
//----------------------------------------------------------------------

bool
Machine::ReadMem(int addr, int size, int *value)
{
    int data;
    ExceptionType exception;
    int physicalAddress;
    
    DEBUG(dbgAddr, "Reading VA " << addr << ", size " << size);
    
    exception = Translate(addr, &physicalAddress, size, FALSE);
    if (exception != NoException) {
	RaiseException(exception, addr);
	return FALSE;
    }
    switch (size) {
      case 1:
	data = mainMemory[physicalAddress];
	*value = data;
	break;
	
      case 2:
	data = *(unsigned short *) &mainMemory[physicalAddress];
	*value = ShortToHost(data);
	break;
	
      case 4:
	data = *(unsigned int *) &mainMemory[physicalAddress];
	*value = WordToHost(data);
	break;

      default: ASSERT(FALSE);
    }
    
    DEBUG(dbgAddr, "\tvalue read = " << *value);
    return (TRUE);
}

//----------------------------------------------------------------------
// Machine::WriteMem
//      Write "size" (1, 2, or 4) bytes of the contents of "value" into
//	virtual memory at location "addr".
//
//   	Returns FALSE if the translation step from virtual to physical memory
//   	failed.
//
//	"addr" -- the virtual address to write to
//	"size" -- the number of bytes to be written (1, 2, or 4)
//	"value" -- the data to be written
//----------------------------------------------------------------------

bool
Machine::WriteMem(int addr, int size, int value)
{
    ExceptionType exception;
    int physicalAddress;
     
    DEBUG(dbgAddr, "Writing VA " << addr << ", size " << size << ", value " << value);

    exception = Translate(addr, &physicalAddress, size, TRUE);
    if (exception != NoException) {
	RaiseException(exception, addr);
	return FALSE;
    }
    switch (size) {
      case 1:
	mainMemory[physicalAddress] = (unsigned char) (value & 0xff);
	break;

      case 2:
	*(unsigned short *) &mainMemory[physicalAddress]
		= ShortToMachine((unsigned short) (value & 0xffff));
	break;
      
      case 4:
	*(unsigned int *) &mainMemory[physicalAddress]
		= WordToMachine((unsigned int) value);
	break;
	
      default: ASSERT(FALSE);
    }
    
    return TRUE;
}

//----------------------------------------------------------------------
// Machine::Translate
// 	Translate a virtual address into a physical address, using 
//	either a page table or a TLB.  Check for alignment and all sorts 
//	of other errors, and if everything is ok, set the use/dirty bits in 
//	the translation table entry, and store the translated physical 
//	address in "physAddr".  If there was an error, returns the type
//	of the exception.
//
//	"virtAddr" -- the virtual address to translate
//	"physAddr" -- the place to store the physical address
//	"size" -- the amount of memory being read or written
// 	"writing" -- if TRUE, check the "read-only" bit in the TLB
//----------------------------------------------------------------------

ExceptionType
Machine::Translate(int virtAddr, int* physAddr, int size, bool writing)
{
    int i;
    unsigned int vpn, offset;
    TranslationEntry *entry;
    unsigned int pageFrame;

    DEBUG(dbgAddr, "\tTranslate " << virtAddr << (writing ? " , write" : " , read"));

// check for alignment errors
    if (((size == 4) && (virtAddr & 0x3)) || ((size == 2) && (virtAddr & 0x1))){
	DEBUG(dbgAddr, "Alignment problem at " << virtAddr << ", size " << size);
	return AddressErrorException;
    }
    // we must have either a TLB or a page table, but not both!
    ASSERT(tlb == NULL || pageTable == NULL);	
    ASSERT(tlb != NULL || pageTable != NULL);	

// calculate the virtual page number, and offset within the page,
// from the virtual address
    vpn = (unsigned) virtAddr / PageSize;
    offset = (unsigned) virtAddr % PageSize;
    
    if (tlb == NULL) {		// => page table => vpn is index into table
	if (vpn >= pageTableSize) {
	    DEBUG(dbgAddr, "Illegal virtual page # " << virtAddr);
	    return AddressErrorException;
	} else if (!pageTable[vpn].valid) {
	    DEBUG(dbgAddr, "Invalid virtual page # " << virtAddr);
	    return PageFaultException;
	}
	entry = &pageTable[vpn];
    } else {
        for (entry = NULL, i = 0; i < TLBSize; i++)
    	    if (tlb[i].valid && (tlb[i].virtualPage == ((int)vpn))) {
		entry = &tlb[i];			// FOUND!
		break;
	    }
	if (entry == NULL) {				// not found
    	    DEBUG(dbgAddr, "Invalid TLB entry for this virtual page!");
    	    return PageFaultException;		// really, this is a TLB fault,
						// the page may be in memory,
						// but not in the TLB
	}
    }

    if (entry->readOnly && writing) {	// trying to write to a read-only page
	DEBUG(dbgAddr, "Write to read-only page at " << virtAddr);
	return ReadOnlyException;
    }
    pageFrame = entry->physicalPage;

    // if the pageFrame is too big, there is something really wrong! 
    // An invalid translation was loaded into the page table or TLB. 
    if (pageFrame >= NumPhysPages) { 
	DEBUG(dbgAddr, "Illegal pageframe " << pageFrame);
	return BusErrorException;
    }
    entry->use = TRUE;		// set the use, dirty bits
    if (writing)
	entry->dirty = TRUE;
    *physAddr = pageFrame * PageSize + offset;
    ASSERT((*physAddr >= 0) && ((*physAddr + size) <= MemorySize));
    DEBUG(dbgAddr, "phys addr = " << *physAddr);
    return NoException;
}
