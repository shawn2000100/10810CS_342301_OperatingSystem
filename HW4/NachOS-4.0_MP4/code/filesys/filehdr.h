// filehdr.h 
//	Data structures for **managing a disk file header**.  
//
//	**A file header describes where on disk to find the data in a file**,
//	along with **other information about the file** (for instance, its
//	length, owner, etc.)

#include "copyright.h"

#ifndef FILEHDR_H
#define FILEHDR_H

#include "disk.h"
#include "pbitmap.h"

#define NumDirect 	((SectorSize - 3 * sizeof(int)) / sizeof(int))  // 原本是 -2 ，改成-3 多存一個指標
#define MaxFileSize 	(NumDirect * SectorSize)

// The following class defines the Nachos "file header" (in UNIX terms, the **"i-node"**), 
// describing where on disk to find all of the data in the file.
// The file header is organized as a simple **table of pointers to data blocks**. 
//
// The **file header data structure can be stored in memory or on disk**.
// When it is on disk, it is stored in a single sector -- this means
// that **we assume the size of this data structure to be the same
// as one disk sector**.  **Without indirect addressing, this
// limits the maximum file length to just under 4K bytes**.
//
// **There is **no constructor**; rather the file header can be initialized
// by allocating blocks for the file** (if it is a new file), 
// **or by reading it from disk**.

class FileHeader {
  public:
	  // MP4 mod tag
	  FileHeader(); // dummy constructor
	  ~FileHeader();
     
	  // Initialize a file header, including **allocating space on disk for the file data**
    bool Allocate(PersistentBitmap *bitMap, int fileSize);
    
    // De-allocate this file's data blocks
    void Deallocate(PersistentBitmap *bitMap);

    // Initialize file header **from disk**
    void FetchFrom(int sectorNumber); 
   	
    // Write modifications to file header back **to disk**
    void WriteBack(int sectorNumber);

    // Convert a byte offset into the file to the disk sector containing the byte
    int ByteToSector(int offset);
    
    // Return the length of the file in bytes
    int FileLength();			

    // Print the contents of the file.
    void Print();
    
    // J: MP4新增功能，為了取得下一個Link的指標
    FileHeader* getNextFileHeader() { return nextFileHeader;}
    int HeaderLength(); // 新增，回傳Headr有幾格
    // ******************************************
  private:
	/*
		MP4 hint:
		You will need a data structure to store more information in a header.
		Fields in a class can be separated into disk part and in-core part.
  		**Disk part are data that will be written into disk**.
	  	**In-core part are data only lies in memory, and are used to maintain the data structure of this class**.
		
    In order to implement a data structure, **you will need to add some "in-core" data
		to maintain data structure**.
		
		**Disk Part - numBytes, numSectors, dataSectors occupy exactly 128 bytes and will be
		written to a sector on disk**.
		In-core part - none
	*/
    // J: MP4新增 *******************************
    FileHeader* nextFileHeader;  // (in-core) 這個要永遠放在第一格!!!!!!
    // ******************************************
    int numBytes;		  	         // Number of bytes in the file
    int numSectors;			         // Number of data sectors in the file
    int dataSectors[NumDirect];  // Disk sector numbers for each data block in the file
    // J: MP4新增 *******************************
    int nextFileHeaderSector;    // (in-disk) 
    // ******************************************
};

#endif // FILEHDR_H
