#include "syscall.h"
#include "system.h"

void handle_SC_ReadInt() {
	// Input: Keyboard
	// Output: Write answer into r2
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
	if(buffer[0] == '-') // I think you know...
	{
		isNegative = true;
		firstIndex = 1;
		lastIndex = 1;                        			   		
	}

	// Checking if the char* is a valid interger
	for(int i = firstIndex; i < numbytes; i++)					
	{
		if(buffer[i] == '.') /// Hypothetically: 125.0000000 is technically an interger 
		{
	    	int j = i + 1;
	    	for(; j < numbytes; j++)
	    	{
			if(buffer[j] != '0') // Indicating this is a float
			{
		    	printf("\n\n The integer number is not valid");
		    	DEBUG('a', "\n The integer number is not valid");
		    	machine->WriteRegister(2, 0);
		    	delete buffer;
			interrupt->Halt();
		    	return;
			}
	    	}
	    	// If passes the test of .000000
	    	lastIndex = i - 1;				
	    	break;                           
		}
		else if(buffer[i] < '0' && buffer[i] > '9')
		{
	    	printf("\n\n The integer number is not valid");
	    	DEBUG('a', "\n The integer number is not valid");
	    	machine->WriteRegister(2, 0);
	    	delete buffer;
		interrupt->Halt();
	    	return;
		}
		lastIndex = i;    
	}			

	// Valid interger, turn the char* buffer into numbers
	for(int i = firstIndex; i<= lastIndex; i++)
	{
		number = number * 10 + (int)(buffer[i] - 48); 
	}

	// Make it (+) if is (-)
	if(isNegative)
	{
		number = number * -1;
	}
	machine->WriteRegister(2, number); // Record the final number into r2 (result)
	delete buffer; // For memory perserverance purpose
	interrupt->Halt();
	return;		
}

