#include "syscall.h"
// #include <stdio.h>
// #include "system.h"
int main() {
    char* name = "tmpF.txt";
    ////////////////////////////////
	int createF_succ = CreateF(name);
	////////////////////////////////
    int id1;
    int id2;
    int id3;
    char* buffer = "noi dung file day ne em gi oi day la file cua minh ne em oi";
    ////////////////////////////////
    id1 = OpenF("filename.txt", 0);
    id2 = OpenF("stdin", 2);
    id3 = OpenF(name, 0);
    ////////////////////////////////
    WriteF(buffer, 10, id3);
    WriteF(buffer, 100, id3);
    
	ReadF(buffer, 10, id1);
	ReadF(buffer, 10, id1);
	ReadF(buffer, 10, id1);
	ReadF(buffer, 10, id1);
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
