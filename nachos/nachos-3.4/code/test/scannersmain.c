#include "syscall.h"
#include "copyright.h"

int main() {
  int passengersPID, scannersPID, semaphoreCheck;
  char *saveWeight = "                                                                                                                         ";
  int saveSize = -1;
  int readInput, numPoint, writeOutput, fileStatus, writePassengers, readScannerId, readSuccess, idSave;
  char tempbuffer; 
// -------------------------------------------------------
  semaphoreCheck = CreateSemaphore("main", 0);
  if (semaphoreCheck == -1) {
    PrintString("Loi tao semaphore\n");
    return 0;
  }
  semaphoreCheck = CreateSemaphore("passengers", 0);
  if (semaphoreCheck == -1) {
    PrintString("Loi tao semaphore\n");
    return 0;
  }
  semaphoreCheck = CreateSemaphore("scanners", 0);
  if (semaphoreCheck == -1) {
    PrintString("Loi tao semaphore\n");
    return 0;
  }
  semaphoreCheck = CreateSemaphore("maincounter", 0);
  if (semaphoreCheck == -1) {
    PrintString("Loi tao semaphore\n");
    return 0;
  }

  readInput = OpenF("input.txt", 0);
  if (readInput == -1){
    PrintString("Khong the doc file input \n");
    return 0;
  }
  numPoint = 0;
  while (1) { //while True but C doesn"t support boolean
    ReadF(&tempbuffer, 1, readInput);
    if (tempbuffer == '\n') break;
    else {
      if (tempbuffer >= '0' && tempbuffer <= '9')
        numPoint = numPoint * 10 + (tempbuffer - '0');
    }
  }
  PrintString("Gia tri so la: ");
  PrintInt(numPoint);
  PrintString("\n");

  fileStatus = CreateF("output.txt");
  if (fileStatus == -1) {
    PrintString("Loi tao file output.txt\n");
    return 0;
  }

  writeOutput = OpenF("output.txt", 0);
  if (writeOutput == -1) {
    CloseF(readInput);
    PrintString("Khong mo file output.txt de viet ket qua \n");
    return 0;
  }

  passengersPID = Exec("./test/passengers");
  if (passengersPID == -1) return 0;
  scannersPID = Exec("./test/scanners");
  if (scannersPID == -1) return 0;
  //Join(passengersPID);
  //Join(scannersPID);

  while (numPoint--) {
    fileStatus = CreateF("pasengers.txt");
    if (fileStatus == -1) {
      CloseF(readInput);
      CloseF(writeOutput);
      PrintString("Khong the tao file passengers.txt\n");
      return 0;
    }
    writePassengers = OpenF("passengers.txt",0);
    if (writePassengers == -1) {
      CloseF(readInput);
      CloseF(writeOutput);
      PrintString("Loi mo file passengers.txt\n");
      return 0;
    }
    // Doc file input.txt va ghi noi dung vao file passengers.txt
    PrintString("Lan handle thu: ");
    PrintInt(numPoint);
    PrintString("\n");
    while (1)
    {
      readSuccess = ReadF(&tempbuffer,1,readInput);
      if (readSuccess <= 0) break;
      // handle reprint weight 
      saveSize = saveSize + 1;
      saveWeight[saveSize] = tempbuffer;
      // ghi noi dung vao file passengers.txt
      if (tempbuffer == '\n') break;
      else WriteF(&tempbuffer, 1, writePassengers);
      
    }
    CloseF(writePassengers);
    Up("passengers");
    Down("main");
    readScannerId = OpenF("scannerid.txt", 0);
    if (readScannerId == -1) {
      CloseF(readInput);
      CloseF(writeOutput);
      return 1;
    }
    idSave = 0;
    while (1) {
	
      int readStatus = ReadF(&tempbuffer, 1, readScannerId);
      if (readStatus < 0) {
        WriteF("\r\n",2, writeOutput);
        CloseF(readScannerId);
        Up("maincounter");
        break;
      }

      WriteF(&tempbuffer, 1, writeOutput);
      WriteF("       ", 7, writeOutput);
    }
  }
  CloseF(readInput);
  CloseF(writeOutput);
  return 0;
}