#include "syscall.h"
int main() {
    char name[] = "filename.txt";
    printf(CreateFile(name));
    Halt();
}