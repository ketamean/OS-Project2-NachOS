#include "syscall.h"

int main() {
    	OpenFileId file;
    	int i;
    	int j = 0;
    	char table[170];
    
    	CreateF("ascii.txt");
    	file = OpenF("ascii.txt", 0);
    	PrintString("\nBANG MA ASCII\n");
    	for (i = 32; i <= 127; i++)
    	{
        	PrintInt(i);
        	PrintChar('\t');
        	PrintChar(i);
        	PrintChar('\n');
        	table[j] = i;
        	j++;
        	table[j] = '\n';
        	j++;
    	}
    	WriteF(table, 200, file);
    	CloseF(file);
    	Halt();
}