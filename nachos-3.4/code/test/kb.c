#include "syscall.h"

int main() {
	int answer = ReadInt();
	PrintInt(answer);
	
	/* float f = ReadFloat();
	PrintFloat(f); */
	
	/* char c = ReadChar();
	PrintChar(c); */

	/* char s[10];
	ReadString(s, 10);
	PrintString(s); */

	Halt();
	return 0;
}
