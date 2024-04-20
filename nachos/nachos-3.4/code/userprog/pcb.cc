#include "pcb.h"
#include "utility.h"
#include "system.h"
#include "thread.h"
#include "addrspace.h"

PCB::PCB(int id)
{
	
	pid= id;
	
	if(id)
		parentID= currentThread->processID;
	else
		parentID= 0;
	
	numwait= 0;
	exitcode= 0;
	thread= NULL;
	joinsem= new Semaphore("JoinSem",0);
	exitsem= new Semaphore("ExitSem",0);
	mutex= new Semaphore("Mutex",1);

	JoinStatus= -1;
}

PCB::~PCB()
{
	if(joinsem != NULL)
		delete joinsem;
	if(exitsem != NULL)
		delete exitsem;
	if(mutex != NULL)
		delete mutex;
	if(thread != NULL) {
		thread->FreeSpace();
		thread->Finish();
	}
}

//------------------------------------------------------------------
int PCB::GetID()
{
	return thread->processID;
}

int PCB::GetNumWait()
{
	return numwait;
}

int PCB::GetExitCode()
{
	return exitcode;	
}

void PCB::SetExitCode(int ec)
{
	exitcode = ec;
}

void PCB::IncNumWait()
{
	// mutex->P();
	++numwait;
	// mutex->V();
}

void PCB::DecNumWait()
{
	// mutex->P();
	if(numwait)
		--numwait;
	// mutex->V();
}

char* PCB::GetNameThread()
{
	return thread->getName();
}

//-------------------------------------------------------------------
// call JoinWait() to block the current process until JoinRealease is invoked
void PCB::JoinWait()
{
	JoinStatus= parentID;
	IncNumWait();
	joinsem->P();
}

// release the blocked process that called JoinWait()
void PCB::JoinRelease()
{
	DecNumWait();
	joinsem->V();
}

void PCB::ExitWait()
{
	exitsem->P();
}

void PCB::ExitRelease()
{
	exitsem->V();
}

//------------------------------------------------------------------
int PCB::Exec(char *filename, int pID)
{	
	// call mutex to avoid running 2 process at a time
	mutex->P();
	thread= new Thread(filename);
	if(thread == NULL)
	{
		printf("\nPCB::Exec: cannot create new thread\n");
		mutex->V();
		return -1;
	}
	thread->processID= pID;

	// parent id of the current thread is the thread which invokes Exec
	parentID = currentThread->processID;

	// call Fork => type-cast Thread to int
	thread->Fork(MyStartProcess,this->thread->processID);
	
	mutex->V();
	return pID;
}


//*************************************************************************************
void MyStartProcess(int pID)
{
	char *filename= pTab->GetName(pID);
	AddrSpace *space= new AddrSpace(filename);
	if(space == NULL)
	{
		printf("\nLoi: Khong du bo nho de cap phat cho tien trinh !!!\n");
		return; 
	}
	currentThread->space= space;

	space->InitRegisters();		// set the initial register values
	space->RestoreState();		// load page table register

	machine->Run();			// jump to the user progam
	ASSERT(FALSE);			// machine->Run never returns;
						// the address space exits
						// by doing the syscall "exit"
}