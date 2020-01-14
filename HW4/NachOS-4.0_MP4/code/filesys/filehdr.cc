// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a **fixed size
//	table of pointers** -- **each entry in the table points to the 
//	disk sector** containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). **The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector**, 
//
//      Unlike in a real system, **we do not keep track of file permissions, 
//	ownership, last modification date, etc.**, in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk

#include "copyright.h"

#include "filehdr.h"
#include "debug.h"
#include "synchdisk.h"
#include "main.h"

//----------------------------------------------------------------------
// MP4 mod tag
// FileHeader::FileHeader
//	There is no need to initialize a fileheader,
//	since all the information should be initialized by Allocate or FetchFrom.
//	The purpose of this function is to keep valgrind happy.
//----------------------------------------------------------------------
FileHeader::FileHeader()
{
  // MP4 add
  nextFileHeader = NULL;
	nextFileHeaderSector = -1;
	// end
  numBytes = -1;
	numSectors = -1;
	memset(dataSectors, -1, sizeof(dataSectors));
}

//----------------------------------------------------------------------
// MP4 mod tag
// FileHeader::~FileHeader
//	Currently, there is not need to do anything in destructor function.
//	However, if you decide to add some "in-core" data in header
//	Always remember to deallocate their space or you will leak memory
//----------------------------------------------------------------------
FileHeader::~FileHeader()
{
  // MP4 add
	if (nextFileHeader != NULL)
		delete nextFileHeader;
  // end
}

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

// MP4 Modified********************************
bool FileHeader::Allocate(PersistentBitmap *freeMap, int fileSize)
{ 
    // 本層FH要存的Bytes
    numBytes = fileSize < MaxFileSize ? fileSize : MaxFileSize;
    // 剩下要存的Bytes
    fileSize -= numBytes;
    // 幫本層分配Data Sectors
    numSectors = divRoundUp(numBytes, SectorSize);
    
    if (freeMap->NumClear() < numSectors)
	    return FALSE;		// not enough space

    for (int i = 0; i < numSectors; i++) {
    	dataSectors[i] = freeMap->FindAndSet();
    	ASSERT(dataSectors[i] >= 0);  // since we checked that there was enough free space, we expect this to succeed
    }
    // 分配完本層，再多分配一個Link指向下一個FH
    if (fileSize > 0) {
        nextFileHeaderSector = freeMap->FindAndSet();  // 為了確認有沒有Free Sector
        if (nextFileHeaderSector == -1)  // 沒有Free Sector
 			      return FALSE;
  		  else {
 			      nextFileHeader = new FileHeader;
 			      return nextFileHeader->Allocate(freeMap, fileSize);
        }
	  }
    return TRUE;
}
// end*******************************************

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------
// MP4 Modified********************************
void FileHeader::Deallocate(PersistentBitmap *freeMap)
{
    for (int i = 0; i < numSectors; i++) {
      ASSERT(freeMap->Test((int)dataSectors[i])); // ought to be marked!
      freeMap->Clear((int)dataSectors[i]);
  	}
  	if (nextFileHeaderSector != -1)
  	{
 		  ASSERT(nextFileHeader != NULL);
 		  nextFileHeader->Deallocate(freeMap);
  	}
}
// end*******************************************

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------
// MP4 Modified********************************
void FileHeader::FetchFrom(int sector)
{
    //DEBUG(dbgFile, "Test[J]: kernel->synchDisk->ReadSector");
    kernel->synchDisk->ReadSector(sector, ((char *)this) + sizeof(FileHeader*));
    //DEBUG(dbgFile, "Test[J]: Done");
    
    if (nextFileHeaderSector != -1)
	  { 
		    nextFileHeader = new FileHeader;
		    nextFileHeader->FetchFrom(nextFileHeaderSector);
	  }
	/*
		MP4 Hint:
		After you add some in-core informations, you will need to rebuild the header's structure
	*/
}
// end*******************************************

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------
// MP4 Modified********************************
void FileHeader::WriteBack(int sector)
{
    kernel->synchDisk->WriteSector(sector, ((char *)this) + sizeof(FileHeader*)); 
	  
    if (nextFileHeaderSector != -1)
	  {
        ASSERT(nextFileHeader != NULL);
		    nextFileHeader->WriteBack(nextFileHeaderSector);
	  }
	/*
		MP4 Hint:
		After you add some in-core informations, you may not want to write all fields into disk.
		Use this instead:
		char buf[SectorSize];
		memcpy(buf + offset, &dataToBeWritten, sizeof(dataToBeWritten));
		...
	*/
}
// end*******************************************

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------
// MP4 **************************************************
int FileHeader::ByteToSector(int offset)
{
    int index = offset / SectorSize;
  	if (index < NumDirect)
  		 return (dataSectors[index]);
  	else
  	{
  		 ASSERT(nextFileHeader != NULL);
  		 return nextFileHeader->ByteToSector(offset - MaxFileSize);
  	}
}
// end **************************************************

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
    
    for (i = 0; i < numSectors; i++)
    	printf("%d ", dataSectors[i]);
    
    printf("\nFile contents:\n");
    for (i = k = 0; i < numSectors; i++) {
	    kernel->synchDisk->ReadSector(dataSectors[i], data);
      for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
	    if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
		    printf("%c", data[j]);
      else
		    printf("\\%x", (unsigned char)data[j]);
	    }
      printf("\n"); 
    }
    delete [] data;
}
