#include "syscall.h"
#include "system.h"

/* --NOTICE: FOR MORE THAN ONE TASKS PERFORMANCE, REPLACE 'interrupt->Halt()' WITH A R INCREMENTER-- */

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
		else if (buffer[i] < '0' && buffer[i] > '9')
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
	interrupt->Halt();
	return;		
}

void handle_SC_PrintInt() {
	// Input: ONE integer, retrieved from r4 using machine->ReadRegister(4).
	// Output: Print ONE integer onto the Console
	int number = machine->ReadRegister(4); 
	if (number == 0) 
	{
		gSynchConsole->Write("0", 1); // print 0
		interrupt->Halt();
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
		gSynchConsole->Write(buffer, numLength + 1); // +1 to counter for the '-' 
		delete buffer;
		interrupt->Halt();
		return;
	}
	buffer[numLength] = 0;	
	gSynchConsole->Write(buffer, numLength);
	delete buffer;
	interrupt->Halt();
	return;        
}


// The process of writing the float functions is pretty same-y with the differences being in tracking decimal '.'
void handle_SC_ReadFloat() {
    // Input: Keyboard
    // Output: Write answer into r2
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
                printf("\n\n The number is not a valid float");
                DEBUG('a', "\n The number is not a valid float");
                machine->WriteRegister(2, 0);
                delete buffer;
                interrupt->Halt();
                return;
            }
            //lastIndex = i - 1; // commented out because we want to allow for trailing zeroes
        }
        else if ((buffer[i] < '0' || buffer[i] > '9') && buffer[i] != '-') // Allow digits and negative sign
        {
            printf("\n\n The number is not a valid float");
            DEBUG('a', "\n The number is not a valid float");
            machine->WriteRegister(2, 0);
            delete buffer;
            interrupt->Halt();
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
    machine->WriteRegisterF(2, number); // Record the final number into f2 (result)
    delete buffer; // For memory preservation purpose
    interrupt->Halt();
    return;     
}

void handle_SC_PrintFloat() {
    // Input: ONE float, retrieved from f4 using machine->ReadRegisterF(4).
    // Output: Print ONE float onto the Console
    float number = machine->ReadRegisterF(4);
    if (number == 0.0f) {
        gSynchConsole->Write("0.0", 3); // print 0.0
        interrupt->Halt();
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
    gSynchConsole->Write(buffer, totalLength);
    delete[] buffer;
    interrupt->Halt();
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
		printf("\n\n Only one character allowed");
		DEBUG('a', "\n Only one character allowed");
		machine->WriteRegister(2, 0);
	}
	else if(numBytes == 0) // Empty
	{
		printf("\n\n Empty input");
		DEBUG('a', "\n Empty input");
		machine->WriteRegister(2, 0);
	}
	else
	{
		// Return the char at index = 0 into r2
		char c = buffer[0];
		machine->WriteRegister(2, c);
	}

	delete buffer;
	interrupt->Halt();
	return;
}
void handle_SC_PrintChar() {
	// Input: ONE char, retrieved from f4 using machine->ReadRegisterF(4).
	// Output: Print ONE char onto the Console
	char c = (char)machine->ReadRegister(4);
	gSynchConsole->Write(&c, 1); // 1 byte
	interrupt->Halt();
	return;
}
void handle_SC_ReadString();
void handle_SC_PrintString();	