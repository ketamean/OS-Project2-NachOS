#include "syscall.h"
#include "copyright.h"

int getFileLength(int id) {
  char* tempbuffer = '\n';
  int cnt = 0;
  int isValid;
  while (1) {
    isValid = ReadF(tempbuffer,1, id);
    if (isValid < 0) break;
    cnt = cnt + 1;
  }
  return cnt;
}

int main() {
  int fileStatus, readPassengers, fileLength, writeLugWeight, filePointer, isEndLug;
  char* buffer = '\n';
  // ----------------------------------------
  
  Up("maincounter");
  while(1) {
    Down("passengers");
    fileStatus = CreateF("scannerid.txt");
    if (fileStatus == -1) {
      PrintString("Loi tao file scannerid.txt\n");
      Up("main");
      return 0;
    }
    readPassengers = OpenF("passengers.txt", 1);
    if (readPassengers == -1) {
      PrintString("Loi doc file passengers.txt \n");
      Up("main");
      return 0;
    }
    fileLength = getFileLength(readPassengers);
    CloseF(readPassengers);
    readPassengers = OpenF("passengers.txt", 1);

    fileStatus = CreateF("lugweight.txt");
    if (fileStatus == -1) {
      PrintString("Loi tao file lugweight.txt\n");
      CloseF(readPassengers);
      Up("main");
      return 0;
    }
    writeLugWeight = OpenF("lugweight.txt", "0");
    if (writeLugWeight == -1) {
      PrintString("Loi mo file lugweight.txt\n");
      CloseF(readPassengers);
      Up("main");
      return 0;
    }
    filePointer = 0;
    while (filePointer < fileLength) {

      isEndLug = 0;
      ReadF(buffer, 1, readPassengers);
      if (buffer != " ") {
        WriteF(buffer,1,writeLugWeight);
      } else {
        isEndLug = 1;
      }
      if (filePointer == fileLength - 1) {
        WriteF("\n",1,writeLugWeight);
        isEndLug = 1;
      }

      if (isEndLug == 1)
      {
        CloseF(writeLugWeight);
        Up("scanners");

        Down("passengers");

        fileStatus = CreateF("lugweight.txt");
        if (fileStatus == -1) {
          PrintString("Loi tao file lugweight.txt trong vong lap\n");
          CloseF(readPassengers);
          Up("main");
          return 0;
        }
        writeLugWeight = OpenF("lugweight.txt",0);
        if (writeLugWeight == -1) {
          PrintString("Loi mo file lugweight.txt trong vong lap\n");
          CloseF(readPassengers);
          Up("main");
          return 0;
        }
      }
      filePointer = filePointer + 1;
    }
    Up("main");
  }
  return 0;
}