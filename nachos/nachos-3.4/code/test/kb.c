#include "syscall.h"

int main() {
	int answer = ReadInt();
	float f = ReadFloat();
	char c = ReadChar();
	char s[255];
	int length = ReadInt();
	ReadString(s, length);

	PrintInt(answer);
	PrintFloat(f);
	PrintChar(c);
	PrintString(s);

	Halt();
	return 0;
}
