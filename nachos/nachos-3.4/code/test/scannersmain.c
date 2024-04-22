#include "syscall.h"
#include "copyright.h"

int main() {

  int semaphoreCheck = CreateSemaphore('main', 0);
  if (semaphoreCheck == -1) {
    PrintString("Loi tao semaphore\n");
    return 0;
  }
  semaphoreCheck = CreateSemaphore('passengers', 0);
  if (semaphoreCheck == -1) {
    PrintString("Loi tao semaphore\n");
    return 0;
  }
  semaphoreCheck = CreateSemaphore('scanners', 0);
  if (semaphoreCheck == -1) {
    PrintString('Loi tao semaphore\n');
    return 0;
  }
  semaphoreCheck = CreateSemaphore('maincounter', 0);
  if (semaphoreCheck == -1) {
    PrintString('Loi tao semaphore\n');
    return 0;
  }


  int execCheck = Exec('./test/passengers');
  if (execCheck == -1) return 0;
  execCheck = Exec('./test/scanners');
  if (execCheck == -1) return 0;

  int readInput = OpenF('input.txt', 1);
  if (readInput == -1){
    PrintString('Khong the doc file input \n');
    return 0;
  }
  int numPoint = 0;
  while (1) { //while True but C doesn't support boolean
    char* buffer='\n';
    ReadF(buffer, 1, readInput);
    if (buffer == '\n') break;
    else {
      if (buffer >= '0' && buffer <= '9')
        numPoint = numPoint * 10 + (buffer - '0');
    }
  }
  int writeOutput = OpenF('output.txt', 0);
  if (writeOutput == -1) {
    CloseF(readInput);
    PrintString('Khong the tao file output de viet ket qua \n');
    return 0;
  }
  while (numPoint--) {
    int fileStatus = CreateF('pasengers.txt');
    if (fileStatus == -1) {
      CloseF(readInput);
      CloseF(writeOutput);
      PrintString('Khong the tao file passengers.txt\n');
      return 0;
    }
    int writePassengers = OpenF('passengers.txt',0);
    if (writePassengers == -1) {
      CloseF(readInput);
      CloseF(writeOutput);
      PrintString('Loi mo file passengers.txt\n');
      return 0;
    }
    // Doc file input.txt va ghi noi dung vao file passengers.txt
    char *saveWeight = '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000';
    int saveSize = -1;
    while (1)
    {
      char* buffer = '\n';
      int readSucess = ReadF(buffer,1,readInput);
      if (readSucess <= 0) break;
      // handle reprint weight 
      saveSize = saveSize + 1;
      saveWeight[saveSize] = buffer;
      // ghi noi dung vao file passengers.txt
      if (buffer == '\n') break;
      else WriteF(buffer, 1, writePassengers);
      
    }
    CloseF(writePassengers);
    Signal("passengers");
    Wait('main');
    int readScannerId = OpenF('scannerid.txt', 1);
    if (readScannerId == -1) {
      CloseF(readInput);
      CloseF(writeOutput);
      return 1;
    }
    int idSave = 0;
    while (1) {
      char *buffer = '\0';
      int readStatus = ReadF(buffer, 1, readScannerId);
      if (readStatus < 0) {
        Write('\r\n',2, writeOutput);
        CloseF(readScannerId);
        Signal('maincounter');
        break;
      }
      
      while (1) {
        if (saveWeight[idSave] != ' ' || saveWeight[idSave] != '\n') {
          WriteF(saveWeight[idSave], 1, writeOutput);
          idSave = idSave + 1;
        } else {
          WriteF(' ',1, writeOutput);
          idSave = idSave + 1;
        }
      }
      WriteF(buffer, 1, writeOutput);
      WriteF('       ', 7, writeOutput);
    }
  }
  CloseF(readInput);
  CloseF(writeOutput);
  return 0;
}