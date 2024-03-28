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

/* R INCREMENT COUNTER */

// ADDED SO THAT I CAN RUN PROCESSES (EX: READ__ AND PRINT__) ONE AFTER ANOTHER
// WITHOUT HAVING TO RESTART THE PROGRAM
// How to use: IncrementR() after each syscall to increment the R counter, allowing the program to continue.
// Only use this if you want to run multiple syscalls in a row. If you only want to run one syscall, you don't need to use this.
// Or: when there is an error, use this to increment the R counter before calling interrupt->Halt() to prevent the program from stopping.
// Else: not desired, just call interrupt->Halt() to stop the program.
void IncrementR()
{
	int counter = machine->ReadRegister(PCReg);
   	machine->WriteRegister(PrevPCReg, counter);
    	counter = machine->ReadRegister(NextPCReg);
    	machine->WriteRegister(PCReg, counter);
   	machine->WriteRegister(NextPCReg, counter + 4);
}


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
#define MAX_FILENAME_LEN 200


/* --NOTICE: FOR MORE THAN ONE TASKS PERFORMANCE, REPLACE 'interrupt->Halt()' WITH A R INCREMENTER-- */

void handle_SC_ReadInt() {
	// Input: Keyboard
	// Output: Write answer into r2
	printf("\nInput: ");
	char* buffer;
	int MAX_BUFFER = 255;
	buffer = new char[MAX_BUFFER + 1];
	int numbytes = gSynchConsole->Read(buffer, MAX_BUFFER); // Read the buffer char string and return it <= MAX_BUFFER (of course)
	// using gSynchConsole from system.h: access between console and users.
	int number = 0; // Answer (if any)
			
	/* THIS IS WHERE THE MAGIC HAPPENS: TURNING CHAR* INTO INTERGER IF VALID */

	// Set a bool for later (-) number handle                      
	bool isNegative = false; 
	int firstIndex = 0;
	int lastIndex = 0;
	if (buffer[0] == '-') // I think you know...
	{
		isNegative = true;
		firstIndex = 1;
		lastIndex = 1;                        			   		
	}

	// Checking if the char* is a valid interger
	for (int i = firstIndex; i < numbytes; i++)					
	{
		if (buffer[i] == '.') /// Hypothetically: 125.0000000 is technically an interger 
		{
	    	int j = i + 1;
	    	for(; j < numbytes; j++)
	    	{
			if (buffer[j] != '0') // Indicating this is a float
			{
		    	printf("\n The integer number is not valid \n");
		    	DEBUG('a', "\n The integer number is not valid \n");
		    	machine->WriteRegister(2, 0);
				delete buffer;
				//IncrementR();
				//interrupt->Halt();
                return;
			}
	    	}
	    	// If passes the test of .000000
	    	lastIndex = i - 1;				
	    	break;                           
		}
		else if (buffer[i] < '0' && buffer[i] > '9')
		{
	    	printf("\n The integer number is not valid \n");
	    	DEBUG('a', "\n The integer number is not valid \n");
	    	machine->WriteRegister(2, 0);
			delete buffer;
			//IncrementR();
			//interrupt->Halt(); // Halt() because of invalid input
			return;
		}
		lastIndex = i;    
	}			

	// Valid integer, turn the char* buffer into numbers
	for (int i = firstIndex; i<= lastIndex; i++)
	{
		number = number * 10 + (int)(buffer[i] - 48); 
	}

	// Make it (+) if is (-)
	if (isNegative)
	{
		number = number * -1;
	}
	machine->WriteRegister(2, number); // Record the final number into r2 (result)
	delete buffer; // For memory perserverance purpose
	// No Halt() because of successful input (to continue the program)
	return;
}

void handle_SC_PrintInt() {
	// Input: ONE integer, retrieved from r4 using machine->ReadRegister(4).
	// Output: Print ONE integer onto the Console
	int number = machine->ReadRegister(4); 
	if (number == 0) 
	{
		printf("\nOutput: \n");
		gSynchConsole->Write("0", 1); // print 0
		//IncrementR();
        return;     
	}
	
	bool isNegative = false; 
	int numLength = 0; // n numbers in num (length)
	int firstIndex = 0; 

	if (number < 0)
	{
		isNegative = true;
		number = number * -1;
		firstIndex = 1; 
	} 	
	
	// length counter (+1 for each /10 is still whole) 
	int tmp = number; 
	while(tmp)
	{
		numLength++;
		tmp /= 10;
	}

	/* THIS IS WHERE THE MAGIC HAPPENS: TURNING CHAR* INTO INTERGER */
	char* buffer;
	int MAX_BUFFER = 255;
	buffer = new char[MAX_BUFFER + 1];
	for (int i = firstIndex + numLength - 1; i >= firstIndex; i--)
	{
		buffer[i] = (char)((number % 10) + 48); // converting each digit to its ASCII representation and storing it in the buffer
		number /= 10;
	}

	if (isNegative)
	{
		buffer[0] = '-';
		buffer[numLength + 1] = 0;
		printf("\nOutput: \n");
		gSynchConsole->Write(buffer, numLength + 1); // +1 to counter for the '-' 
		delete buffer;
		//IncrementR();
        return; 
	}
	buffer[numLength] = 0;	
	printf("\nOutput: \n");
	gSynchConsole->Write(buffer, numLength);
	delete buffer;       
	return;
}


// The process of writing the float functions is pretty same-y with the differences being in tracking decimal '.'
void handle_SC_ReadFloat() {
    // Input: Keyboard
    // Output: Write answer into r2
	printf("\nInput: ");
    char* buffer;
    int MAX_BUFFER = 255;
    buffer = new char[MAX_BUFFER + 1];
    int numbytes = gSynchConsole->Read(buffer, MAX_BUFFER); // Read the buffer char string and return it <= MAX_BUFFER (of course)
    // using gSynchConsole from system.h: access between console and users.
    float number = 0.0f; // Answer (if any)
            
    /* THIS IS WHERE THE MAGIC HAPPENS: TURNING CHAR* INTO FLOAT IF VALID */

    // Set a bool for later (-) number handle                      
    bool isNegative = false; 
    int firstIndex = 0;
    int lastIndex = 0;
    int dotCount = 0; // Track the number of dots encountered
    if (buffer[0] == '-') // I think you know...
    {
        isNegative = true;
        firstIndex = 1;
        lastIndex = 1;                                        
    }

    // Checking if the char* is a valid float
    for (int i = firstIndex; i < numbytes; i++)                 
    {
        if (buffer[i] == '.') /// Check for decimal point
        {
            dotCount++;
            if (dotCount > 1) { // More than one dot is invalid
                printf("\n\n The number is not a valid float \n");
                DEBUG('a', "\n The number is not a valid float \n");
                machine->WriteRegister(2, 0);
                delete buffer;
                //IncrementR();
				//interrupt->Halt();
        		return; 
            }
            //lastIndex = i - 1; // commented out because we want to allow for trailing zeroes
        }
        else if ((buffer[i] < '0' || buffer[i] > '9') && buffer[i] != '-') // Allow digits and negative sign
        {
            printf("\n\n The number is not a valid float \n");
            DEBUG('a', "\n The number is not a valid float \n");
            machine->WriteRegister(2, 0);
            delete buffer;
            //IncrementR();
			//interrupt->Halt();
        	return; 
        }
        lastIndex = i;    
    }           

    // Valid float, turn the char* buffer into numbers
    float multiplier = 1.0f;
    for (int i = lastIndex; i >= firstIndex; i--) // Starting from the end to handle decimals correctly
    {
        if (buffer[i] == '.') // Skip the dot
            continue;
        if (buffer[i] == '-') // Skip the negative sign
            continue;
        number += (buffer[i] - '0') * multiplier; // Convert digit to float and add to the number
        multiplier *= 10.0f; // Update multiplier for next digit place
    }

    // Make it (+) if is (-)
    if (isNegative)
    {
        number *= -1.0f;
    }
    machine->WriteRegister(2, number); // Record the final number into r2 (result)
    delete buffer; // For memory preservation purpose   
	return;
}

void handle_SC_PrintFloat() {
    // Input: ONE float, retrieved from r4 using machine->ReadRegister(4).
    // Output: Print ONE float onto the Console
    float number = machine->ReadRegister(4);
    if (number == 0.0f) {
		printf("\nOutput: \n");
        gSynchConsole->Write("0.0", 3); // print 0.0, the 3 is the number of bytes to write
        //IncrementR();
        return;   
    }

    bool isNegative = false;
    if (number < 0.0f) {
        isNegative = true;
        number = -number; // Convert negative to positive for easier processing
    }

    // Determine the length of the integer part of the number
    int intPartLength = (int)number;
    int intLength = 0;
    do {
        intLength++;
        intPartLength /= 10;
    } while (intPartLength != 0);

    // Determine the length of the fractional part
    int fracLength = 6; // Default to 6 decimal places
    float fracPart = number - (int)number;
    for (int i = 0; i < 6; ++i) {
        fracPart *= 10.0f;
        if ((int)fracPart == 0) {
            fracLength = i;
            break;
        }
    }

    // Total length of the buffer
    int totalLength = intLength + 1 + fracLength; // +1 for the decimal point

	/* THIS IS WHERE THE MAGIC HAPPENS: TURNING CHAR* INTO FLOAT */
    char* buffer = new char[totalLength + 1]; // +1 for null terminator
    int idx = 0;

    // Convert the integer part to characters
    do {
        buffer[idx++] = '0' + (int)number % 10;
        number /= 10;
    } while ((int)number != 0);

    // Reverse the integer part in the buffer
    for (int i = 0; i < intLength / 2; ++i) {
        char temp = buffer[i];
        buffer[i] = buffer[intLength - i - 1];
        buffer[intLength - i - 1] = temp;
    }

    // Add decimal point
    buffer[intLength] = '.';

    // Convert the fractional part to characters
    for (int i = 0; i < fracLength; ++i) {
        buffer[intLength + 1 + i] = '0' + (int)fracPart % 10;
        fracPart /= 10;
    }

    // Add null terminator
    buffer[totalLength] = '\0';

    // Add negative sign if needed
    if (isNegative) {
        buffer[0] = '-';
    }

    // Print the buffer to console
	printf("\nOutput: \n");
    gSynchConsole->Write(buffer, totalLength);
    delete[] buffer;
	return;
}

void handle_SC_ReadChar() {
	//Input: Keyboard
	//Output: ONE char to r2
	int maxBytes = 255;
	char* buffer = new char[255];
	int numBytes = gSynchConsole->Read(buffer, maxBytes);

	if(numBytes > 1) // Not valid if >1 char
	{
		printf("\n\n Only one character allowed \n");
		DEBUG('a', "\n Only one character allowed \n");
		machine->WriteRegister(2, 0);
		interrupt->Halt();
	}
	else if(numBytes == 0) // Empty
	{
		printf("\n\n Empty input \n");
		DEBUG('a', "\n Empty input \n");
		machine->WriteRegister(2, 0);
		interrupt->Halt();
	}
	else
	{
		// Return the char at index = 0 into r2
		char c = buffer[0];
		machine->WriteRegister(2, c);
	}
	delete buffer;
	return;
}

void handle_SC_PrintChar() {
	// Input: ONE char, retrieved from f4 using machine->ReadRegisterF(4).
	// Output: Print ONE char onto the Console
	char c = (char)machine->ReadRegister(4);
	printf("\nOutput: \n");
	gSynchConsole->Write(&c, 1); // 1 byte
	return;
}

void handle_SC_ReadString() {
	// Input: Buffer(char*), do dai toi da cua chuoi nhap vao(int)
	// Output: String to 
	int virtAddr, length;
	char* buffer;
	virtAddr = machine->ReadRegister(4); // Get address of the buffer in User Space, retrieved from r4
	length = machine->ReadRegister(5); // Length input of the buffer, retrieved from r5

	/* FIRST APPERANCE OF User2System - System2User methods */
	buffer = User2System(virtAddr, length); // Copy string from User Space to System Space
	gSynchConsole->Read(buffer, length); // Read it (duh)
	System2User(virtAddr, length, buffer); // Copy it back to User Space
	delete buffer; 
	return;
}

void handle_SC_PrintString() {
	// Input: Buffer(char*)
	// Output: Print Buffer onto the Console
	int virtAddr;
	char* buffer;
	virtAddr = machine->ReadRegister(4); // Get address of the buffer in User Space, retrieved from r4

	buffer = User2System(virtAddr, 255); // Copy string from User Space to System Space
	int length = 0; // Length of the string (default 0)
	while (buffer[length] != 0) {
		length++; // Count the length of the string
	}
	printf("\nOutput: \n");
	gSynchConsole->Write(buffer, length + 1); // Write the string to the Console (length + 1 for the null terminator)
	delete buffer; 
	return;
}

void handle_SC_CreateFile() {
// Input:
//		Reg 4: address of file name in user memory space
// Output:
//		Reg 2: 0 if success; otherwise, -1
	int virtualAddr = machine->ReadRegister(4);
	printf("Reading file name...\n");
	char* fname = User2System(virtualAddr, MAX_FILENAME_LEN);
	if (!fname) {
		printf("Not enough memory in system!\n");
		machine->WriteRegister(2, -1); 	// error
		delete[] fname;
		return;
	} else if (strlen(fname) == 0) {
		printf("File name is invalid: file name cannot be empty\n");
		machine->WriteRegister(2, -1); 	// error
		delete[] fname;
		return;
	}

	printf("Finish reading file name.\n");
	printf("Creating file: <%s>...\n", fname);

	int succ = fileSystem->Create(fname, 0);
	// args: filename = fname, initialSize = 0
	
	if (succ == -1) {
		printf("There is no memory space for new file.\n");
		machine->WriteRegister(2, -1); 	// error
	}
	printf("Finish creating file: <%s>\n", fname);
	machine->WriteRegister(2, 0);	// success
	delete[] fname;
}

void handle_SC_ReadFile() {
    
} 


void handle_SC_WriteFile() {
    
}


void handle_SC_CloseFile() {
    
}


void handle_SC_OpenFile() {
// Input:
//		Reg 4: address of file name in user memory space
//		Reg 5: type of file opening (0 for complete access - read and write; 1 for read-only)
// Output:
//		Reg 2: OpenFileID which is the index of the opening file in the file table, in range [0,9] (10 files in total); or -1 if an error occurs
	int virAddr_name = machine->ReadRegister(4);
	int type = machine->ReadRegister(5);
	if (type == 2) {
		machine->WriteRegister(2, 0);
		printf("Open stdin.\n");
		return;
	} else if (type == 3) {
		machine->WriteRegister(2, 1);
		printf("Open stdout.\n");
		return;
	} else if (type != 0 && type != 1) {
		printf("OpenF: wrong type\n");
		machine->WriteRegister(2, -1); 	// returns error
		return;
	}
	char* fname = User2System(virAddr_name, MAX_FILENAME_LEN);
	if (!fname) {
		printf("Invalid file name: %s\n", fname);
		machine->WriteRegister(2, -1); 	// returns error
		return;
	}
	int f = fileSystem->Open(fname, type);
	if (f == -1) {
		printf("Cannot open file %s\n", fname);
		machine->WriteRegister(2, -1); 	// returns error
		delete[] fname;
		return;
	}
	printf("File is opened with ID <%d>\n", f);
	machine->WriteRegister(2, f); 	// returns OpenFileID
	delete[] fname;
}

/* EXCEPTION HANDLER */

void ExceptionHandler(ExceptionType which) {
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
		case SyscallException:
			switch(type) {

				case SC_Halt:
				// Input: None
				// Output: System Shutdown Call
				DEBUG('a', "\n Shutdown, initiated by user program. ");
				printf("\n Shutdown, initiated by user program. ");
				interrupt->Halt();
				return;

				case SC_CreateFile:
				{
					handle_SC_CreateFile();
					IncrementR();
					break;
				}

				case SC_OpenFile:
				{
					handle_SC_OpenFile();
					IncrementR();
					break;
				}
				case SC_CloseFile:
				{
					handle_SC_CloseFile();
					IncrementR();
					break;
				}
				case SC_ReadFile:
				{
					handle_SC_ReadFile();
					IncrementR();
					break;
				}
				case SC_WriteFile:
				{
					handle_SC_WriteFile();
					IncrementR();
					break;
				}
				case SC_ReadInt:
				{
					handle_SC_ReadInt();
					IncrementR();
					break;
				}
				case SC_PrintInt:
				{
					handle_SC_PrintInt();
					IncrementR();
					break;
				}
				case SC_ReadFloat:
				{
					handle_SC_ReadFloat();
					IncrementR();
					break;
				}
				case SC_PrintFloat:
				{
					handle_SC_PrintFloat();
					IncrementR();
					break;
				}
				case SC_ReadChar:
				{
					handle_SC_ReadChar();
					IncrementR();
					break;
				}
				case SC_PrintChar:
				{
					handle_SC_PrintChar();
					IncrementR();
					break;
				}
				case SC_ReadString:
				{
					handle_SC_ReadString();
					IncrementR();
					break;
				}
				case SC_PrintString:
				{
					handle_SC_PrintString();
					IncrementR();
					break;
				}		
			}
	}
}
