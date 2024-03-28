#include "syscall.h"
// #include <stdio.h>
// #include "system.h"
int main() {
    char* name = "filename.txt";
    // printf("Create File success? %d", CreateF(name));
	CreateF(name);
    OpenF(name, 0);
    Halt();
}
