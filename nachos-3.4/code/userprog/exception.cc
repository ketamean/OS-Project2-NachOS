// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#include "iofile.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------


/* USER SPACE AND KERNEL SPACE ALLOCATION */

// Input: User(int) Space - buffer(int) limit
// Output: A pointer to a character array (char*) containing the copied string.
// Function: Copy a string of characters from user space to kernel space in NachOS. 
char* User2System(int virtAddr, int limit) {
	int idx;
	int oneChar; // hold one char for itterating (duh)
	char* kernelBuf = NULL; 
	// Allocates memory for a character buffer (kernelBuf) in kernel space using the new operator. 
	// The size of the buffer is limit + 1 to accommodate a terminal character ('\0').
	kernelBuf = new char[limit + 1]; // Needed for terminal char*
	if (kernelBuf == NULL)
		return kernelBuf; // NULL => Non-successful allocation
		
	memset(kernelBuf, 0, limit + 1); 
	// Initializes the allocated memory with zeros using the memset function. 
	// This ensures that the memory buffer is properly initialized, especially for cases where the entire buffer may not be filled with data.
	
	for (idx = 0; idx < limit; idx++)
	{
		machine->ReadMem(virtAddr + idx, 1, &oneChar);
		kernelBuf[idx] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf; // NOTICE: This returns the kernelBuf POINTER, which holds the copied string from user space to kernel space.
}

// Input: User(int) Space - buffer(int) limit - buffer(char*) Space
// Output: The number of characters actually copied. If successful, this should be equal to len. 
// Function: Copy a string of characters from kernel space to user space in a NachOS. 
int System2User(int virtAddr, int len, char* buffer) // NOTICE: This has added len parameter
{
	if (len < 0) return -1; 
	if (len == 0)return len; // Your typical errors
	int i = 0;
	int oneChar = 0;
	do{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar); 
		// Writes the character oneChar to the memory location 
		// specified by virtAddr + i in user space using the machine->WriteMem function.
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

// max length of file name
#define MAX_FILENAME_LEN 32

// Input:
// 		Reg 4: address of file name in user memory space
// Output:
// 		Reg 2: 0 if success; otherwise, -1
void handle_SC_CreateFile() {
    int virtualAddr = machine->readRegister(4);
	printf('Reading file name...\n');
	char* fname = User2System(virtualAddr, MAX_FILENAME_LEN);
	if (!fname) {
		printf("Not enough memory in system!\n");
		machine->WriteRegister(2, -1); 	// error
		delete[] fname;
		return;
	}

	printf("Finish reading file name.\n");
	printf("File name: <%s>", fname);

	machine->WriteRegister(2, 0);	// success
	fileSystem->
}

void handle_SC_Read() {
    
}


void handle_SC_Write() {
    
}


void handle_SC_Close() {
    
}


void handle_SC_Open() {
    
}

/* EXCEPTION HANDLER */

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2); // This is a global variable for SynchConsole
    // NOTICE: R2 Contains the syscall task and the result(s)/variable(s) that will be return, the rest or just for variables handler

switch (which) {
	case NoException:
		return;

	case PageFaultException:
		DEBUG('a', "\n No valid translation found");
		printf("\n\n No valid translation found");
		interrupt->Halt();
		break;
	case SysCallException:
		switch(type) {

			case SC_Halt:
			// Input: None
			// Output: System Shutdown Call
			DEBUG('a', "\nShutdown, initiated by user program. ");
			printf("\nShutdown, initiated by user program. ");
			interrupt->Halt();
			return;

			case SC_CreateFile:
			{
				handle_SC_CreateFile();
				break;
			}


    /* if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
    } else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    } Old habit dies young*/
}
