#include "syscall.h"

int main() {
    	OpenFileId file;
    	int i;
    	int j = 0;
    	char table[256];
    	// Tao file ascii.txt 
    	CreateF("ascii.txt");
		// Mo file de tien hanh ghi
    	file = OpenF("ascii.txt", 0);
    	PrintString("\nBANG MA ASCII\n");
		// Chay vong lap de in ra ky tu ascii
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
		// Tien hanh ghi vao file
    	WriteF(table, 256, file);
    	CloseF(file);
    	Halt();
}