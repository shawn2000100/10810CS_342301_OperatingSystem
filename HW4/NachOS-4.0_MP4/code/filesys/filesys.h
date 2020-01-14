// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	**A file system is a set of files stored on disk, organized
//	into directories**.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  **Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h)**.
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  **The disk is simulated using the native UNIX 
//	file system (in a file named "DISK")**. 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single **"root" directory**, **listing
//	all of the files in the file system**; unlike UNIX, **the baseline
//	system **does not** provide a hierarchical directory structure**.  
//	In addition, there is a **bitmap** for allocating
//	disk sectors. **Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system** -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

typedef int OpenFileId;

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
class FileSystem {
  public:
    FileSystem() { for (int i = 0; i < 20; i++) fileDescriptorTable[i] = NULL; }

    bool Create(char *name) {
	int fileDescriptor = OpenForWrite(name);

	if (fileDescriptor == -1) return FALSE;
	Close(fileDescriptor); 
	return TRUE; 
	}

    OpenFile* Open(char *name) {
	  int fileDescriptor = OpenForReadWrite(name, FALSE);

	  if (fileDescriptor == -1) return NULL;
	  return new OpenFile(fileDescriptor);
      }

    bool Remove(char *name) { return Unlink(name) == 0; }

	OpenFile *fileDescriptorTable[20];
	
};

// 200112[J]: MP4走這邊
#else // FILESYS

// 200112: 其實MAXOPENFILE會比設定值少1 (0不存)
# define MAXOPENFILES 65

class FileSystem {
  public:
  // Initialize the file system. **Must be called *after* "synchDisk" has been initialized**.
  // If "format", there is nothing on the disk, 
  // so initialize the directory and the bitmap of free blocks.
  // MP4 mod tag
    FileSystem(bool format);		
	
	  ~FileSystem();
    
    // 200112[J]: MP4新增***************************
    bool Create(char *name, int initialSize);     // 跟下面的CreateFile0幾乎一樣，主要是給OS內部用的
    OpenFile* Open(char *name);                   // 跟下面的OpenFile0幾乎一樣，主要是給OS內部用的
    
    int CreateFile0(char* name, int initialSize);  // Create a file (UNIX creat)
    OpenFileId OpenFile0(char* name);              // Open a file (UNIX open)
    int ReadFile0(char* buffer,int initialSize, int id);
    int WriteFile0(char* buffer,int initialSize, int id);
    int CloseFile0(int id);
    OpenFileId PutFileDescriptor(OpenFile* fileDescriptor);

    int fileDescriptorIndex;
    OpenFile* fileDescriptorTable[MAXOPENFILES];
    //************************************************
    
    // Delete a file (UNIX unlink)
    bool Remove(char *name);
    
    // List all the files in the file system
    void List();

    // List all the files and their contents
    void Print();

  private:
    // **Bit map of free disk blocks**, represented as a file
    OpenFile* freeMapFile;
    // **"Root" directory** -- list of file names, represented as a file
    OpenFile* directoryFile;
};

#endif // FILESYS

#endif // FS_H
