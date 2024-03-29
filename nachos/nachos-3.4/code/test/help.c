#include "syscall.h"
#define MAX_LEN 1024 

int main() {
	// Mo file mo ta de doc noi dung
    	OpenFileId file = OpenF("mota.txt", 0);
	// Tao bien buffer de chua noi dung
    	char buffer[MAX_LEN];
	// Tien hanh doc noi dung vao buffer
    	ReadF(buffer, MAX_LEN, file);
	// Thuc hien dong tien trinh doc file
    	CloseF(file);
	// In noi dung doc duoc ra man hinh
    	PrintString(buffer);

    	Halt();
}