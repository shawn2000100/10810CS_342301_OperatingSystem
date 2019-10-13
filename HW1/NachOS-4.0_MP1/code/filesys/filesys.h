// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 

// 1910010[J]: Hint: 這個檔案需要被修改!

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"
#include "debug.h" 		//just for test!!!

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
typedef int OpenFileId;

class FileSystem {
  public:
    FileSystem() { 
	for (int i = 0; i < 20; i++) fileDescriptorTable[i] = NULL; 
    }

    bool Create(char *name) {
	int fileDescriptor = OpenForWrite(name);

	if (fileDescriptor == -1) return FALSE;
	Close(fileDescriptor); 
	return TRUE; 
    }
//The OpenFile function is used for open user program  [userprogram/address.cc]
    OpenFile* Open(char *name) {
	int fileDescriptor = OpenForReadWrite(name, FALSE);
	if (fileDescriptor == -1) return NULL;
	return new OpenFile(fileDescriptor);
    }


//  1910010[J]: 這邊應該是MP1需要實作的關鍵之一!
//  1910010[J]: 這邊的4個function與kernel.h的4個function互相對照，實作的部分可能就是被定義在這邊!
//  1910010[J]: 改的過程可能會需要利用到sysdep.h裡面的功能!?
//  The OpenAFile function is used for kernel open system call
    OpenFileId OpenAFile(char *name) 
    {
    	int fileDescriptor = OpenForReadWrite(name, FALSE);
    	return fileDescriptor;
    }
    
    int WriteFile0(char *buffer, int size, OpenFileId id){ 
    //  191012[J]: 因為跟sysdep裡的WriteFile有命名衝突，而我又不熟namespce，故尾部加上0
      WriteFile(id, buffer, size);
      return size;
    }

/*
    int ReadFile(char *buffer, int size, OpenFileId id){
    }
    int CloseFile(OpenFileId id){
    }
*/


    bool Remove(char *name) { return Unlink(name) == 0; }

    OpenFile *fileDescriptorTable[20];
};

#else // FILESYS
class FileSystem {
  public:
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.

    bool Create(char *name, int initialSize);  	
					// Create a file (UNIX creat)

    OpenFile* Open(char *name); 	// Open a file (UNIX open)

    bool Remove(char *name);  		// Delete a file (UNIX unlink)

    void List();			// List all the files in the file system

    void Print();			// List all the files and their contents

  private:
   OpenFile* freeMapFile;		// Bit map of free disk blocks,
					// represented as a file
   OpenFile* directoryFile;		// "Root" directory -- list of 
					// file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
