#include "syscall.h"

int main() {
	//int i = ReadInt();
	//PrintInt(i);
	
	float f = ReadFloat();
	PrintFloat(f);
	
	//char c = ReadChar();
	//PrintChar(c);

	//char s[255];
	//int length = ReadInt();
	//ReadString(s, length);
	//PrintString(s);

	Halt();
	return 0;
}
