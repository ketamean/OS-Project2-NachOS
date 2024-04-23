#include "syscall.h"
#include "copyright.h"

int main() {
  int scanner1 = 0;
  int scanner2 = 0;
  int scanner3 = 0;
  int writeScannerId, readLugWeight, weight, readEndline, isEnd;
  while (1)
  {
    Down("maincounter");
    writeScannerId = OpenF("scannerid.txt", 0);
    if (writeScannerId == -1) {
      PrintString("Loi mo file scannerid.txt");
      Up("passengers");
    }
    while (1) {
      Down("scanners");
      
      readLugWeight = OpenF("lugweight.txt", 1);
      if (readLugWeight == -1) {
        PrintString("Loi doc file lugweight.txt tu scanners");
        CloseF(writeScannerId);
        Up("passengers");
        return 0;
      }

      weight = 0;
      readEndline = 0;
      while (1) {
        char* buffer = "\n";
        isEnd = ReadF(buffer, 1, readLugWeight);
        if (isEnd < 0) {
          CloseF(readLugWeight);
          break;
        } 
        if (buffer != "\n") {
          weight = weight * 10 + (buffer - "0");
        } else {
          readEndline = 1;
          CloseF(readLugWeight);
          break;
        }
      }
      if (weight != 0) {
        if (scanner1 <= scanner2 && scanner1 <= scanner3) {
          scanner1 += weight;
          WriteF("1",1, writeScannerId);
        } else if (scanner2 <= scanner1 && scanner2 <= scanner3) {
          scanner2 += weight;
          WriteF("2",1,writeScannerId);
        } else {
          scanner3 += weight;
          WriteF("3",1,writeScannerId);
        }
      }
      if (readEndline == 1) {
        scanner1 = 0;
        scanner2 = 0;
        scanner3 = 0;
        CloseF(writeScannerId);
        Up("passengers");
        break;
      }
    }
  }
  
  return 0;
}