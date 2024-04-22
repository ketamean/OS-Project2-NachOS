#include "syscall.h"
#include "copyright.h"

int getFileLength(int id) {
  char* tempbuffer = '\n';
  int cnt = 0;
  while (1) {
    int isValid = ReadF(tempbuffer,1, id);
    if (isValid < 0) break;
    cnt = cnt + 1;
  }
  return cnt;
}

int main() {
  Signal('maincounter');
  while(1) {
    Wait('passengers');
    int fileStatus = CreateF('scannerid.txt');
    if (fileStatus == -1) {
      PrintString("Loi tao file scannerid.txt\n");
      Signal('main');
      return 0;
    }
    int readPassengers = OpenF('passengers.txt', 1);
    if (readPassengers == -1) {
      PrintString('Loi doc file passengers.txt \n');
      Signal('main');
      return 0;
    }
    int fileLength = getFileLength(readPassengers);
    CloseF(readPassengers);
    readPassengers = OpenF('passengers.txt', 1);

    fileStatus = CreateF('lugweight.txt');
    if (fileStatus == -1) {
      PrintString('Loi tao file lugweight.txt\n');
      CloseF(readPassengers);
      Signal('main');
      return 0;
    }
    int writeLugWeight = OpenF('lugweight.txt', '0');
    if (writeLugWeight == -1) {
      PrintString('Loi mo file lugweight.txt\n');
      CloseF(readPassengers);
      Signal('main');
      return 0;
    }
    int filePointer = 0;
    while (filePointer < fileLength) {

      int isEndLug = 0;
      char* buffer = '\n';
      ReadF(buffer, 1, readPassengers);
      if (buffer != ' ') {
        WriteF(buffer,1,writeLugWeight);
      } else {
        isEndLug = 1;
      }
      if (filePointer == fileLength - 1) {
        WriteF('\n',1,writeLugWeight);
        isEndLug = 1;
      }

      if (isEndLug == 1)
      {
        CloseF(writeLugWeight);
        Signal('scanners');

        Wait('passengers');

        fileStatus = CreateF('lugweight.txt');
        if (fileStatus == -1) {
          PrintString('Loi tao file lugweight.txt trong vong lap\n');
          close(readPassengers);
          Signal('main');
          return 0;
        }
        writeLugWeight = OpenF('lugweight.txt',0);
        if (writeLugWeight == -1) {
          PrintString('Loi mo file lugweight.txt trong vong lap\n');
          CloseF(readPassengers);
          Signal('main');
          return 0;
        }
      }
      filePointer = filePointer + 1;
    }
    Signal('main');
  }
  return 0;
}