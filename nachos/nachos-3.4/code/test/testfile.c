#include "syscall.h"
// #include <stdio.h>
// #include "system.h"
int main() {
    char* name = "filename.txt";
    ////////////////////////////////
	int createF_succ = CreateF(name);
	////////////////////////////////
    int id1;
    int id2;
    id1 = OpenF(name, 0);
    id2 = OpenF("stdin", 2);
    ////////////////////////////////
    char* buffer;
	ReadF(buffer, 10, id1);
	////////////////////////////////

	////////////////////////////////
	CloseF(0);
    CloseF(1);
    CloseF(id1);
    CloseF(id2);
	////////////////////////////////
    Halt();
}
