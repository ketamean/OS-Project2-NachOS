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
//	running the Nachos simulation.  This is provided in case the
//	multiprogramming and virtual memory assignments (which make use
//	of the file system) are done before the file system assignment.
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
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"
#define N_FILES_IN_TABLE 100
#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
class FileSystem {
  public:
	OpenFile** file_table;
    FileSystem(bool format) {
		this->file_table = new OpenFile*[N_FILES_IN_TABLE];
		for (int i = 0; i < N_FILES_IN_TABLE; ++i) {
			this->file_table[i] = NULL;
		}
		this->Create("stdin", 0);
		this->Create("stdout", 0);
		this->Open("stdin", 2);
		this->Open("stdout", 3);
	}

	~FileSystem() {
		for (int i = 0; i < N_FILES_IN_TABLE; ++i) {
			if (file_table[i])
				delete file_table[i];
		}
		delete[] file_table;
	}

    bool Create(char *name, int initialSize) {
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

	int Open(char* name, int _type) {
		// return an OpenFile pointer if there are memory spaces remain;
		// otherwise, return NULL
		int freeslot;
		if (_type == 2) {
			freeslot = 0;
		} else if (_type == 3) {
			freeslot = 1;
		} else if (_type == 0 || _type == 1) {
			freeslot = this->getIdxFreeSlot();
		} else {
			printf("FileSystem::Open(char*, int): wrong type");
			freeslot = -1;
		}
		
		if (freeslot == -1) {
			return -1;
		}
		int fileDescriptor = OpenForReadWrite(name, FALSE);
		if (fileDescriptor == -1) return -1;
		this->file_table[freeslot] = new OpenFile(fileDescriptor, _type);
		return freeslot;
	}

	// get index of the free slot with smallest index in the file table
	int getIdxFreeSlot() {
		/*
		returns:
			index [0,9] if there is a free slot
			-1 if there is no free slot
		*/
		for (int i = 2; i < N_FILES_IN_TABLE; ++i) {
			if (!this->file_table[i])
				return i;
		}
		return -1;
	}
    bool Remove(char *name) {
    	return Unlink(name) == 0;
    }

};

#else // FILESYS
class FileSystem {
  public:
	OpenFile** file_table;
	int getIdxFreeSlot(); // get index of the smallest-index free slot in the file table
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.
	~FileSystem();
    bool Create(char *name, int initialSize);
					// Create a file (UNIX creat)

    OpenFile* Open(char *name); 	// Open a file (UNIX open)
	
	int Open(char* name, int _type);	// Open a file (UNIX open) with a file type
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
