#include "ptable.h"
#include "system.h"

PTable::PTable(int size)
{
	// invalid size
	if (size < 0) return;


	int i; // iterative var
	psize = size;
	bm = new BitMap(size);
	bmsem = new Semaphore("bmsem",1);
		
	for(i = 0 ; i < MAXPROCESS ; ++i)
		pcb[i] = NULL;
	bm->Mark(0);

	pcb[0] = new PCB(0); // new PCB with id = 0
	pcb[0]->parentID = -1;
}

PTable::~PTable()
{
	int i=0;
	if(bm!=NULL)
		delete bm;
	if(bmsem!=NULL)
		delete bmsem;
	for(i=0; i<psize; i++)
		if(pcb[i]!=NULL)
			delete pcb[i];
}

//--------------------------------------------------------------------

int PTable::ExecUpdate(char* filename)
{
	bmsem->P();			//chi nap 1 tien trinh vao mot thoi diem

	// check filename validation
	if (filename == NULL) {
		printf("\nPTable::ExecUpdate : Can't not execute if filename is NULL.\n");
		bmsem->V();
		return -1;
	}
	// avoid current thread invokes itself
	if ( strcmp( filename, currentThread->getName() ) == 0 ) {
		printf("\nPTable::ExecUpdate : a process cannot invoke itself.\n");
		bmsem->V();
		return -1;
	}

	// check if file is able to be opened
	OpenFile *executable = fileSystem->Open(filename);
	if (executable == NULL) 
	{
		printf("\nPTable::ExecUpdate: Unable to open file %s.\n", filename);
		bmsem->V();
		return -1;
    }
	delete executable;			// close file
////////////////////////////////////////////////////////////

//Kiem tra con slot trong khong
	int ID= GetFreeSlot();
	if(ID < 0)
	{
		printf("\nPTable::ExecUpdate: Da vuot qua 10 tien trinh !!!\n");
		bmsem->V();
		return -1;
	}
////////////////////////////////////////////////////////////
	printf("PTable::ExecUpdate: file %s, pid = %d.\n", filename, ID);
	pcb[ID]= new PCB(ID);
	bm->Mark(ID);
	pcb[ID]->parentID = currentThread->processID;
	int pID= pcb[ID]->Exec(filename,ID);
	bmsem->V();
	return pID;
}

int PTable::ExitUpdate(int ec)
{
//Kiem tra pID co ton tai khong
	int pID= currentThread->processID;

	//Neu la main process thi Halt()
	if(pID==0)
	{
		interrupt->Halt();
		return 0;
	}

	if(!IsExist(pID))
	{
		printf("\nPTable::ExitUpdate: Tien trinh khong ton tai !!!\n");
		return -1;
	}
//////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////

	pcb[pID]->SetExitCode(ec);
	pcb[ pcb[pID]->parentID ]->DecNumWait();
	
	if(pcb[pID]->JoinStatus != -1)
	{
		pcb[pID]->JoinRelease();
		pcb[pID]->ExitWait();
	}
	Remove(pID);
	return ec;
}

int PTable::JoinUpdate(int pID)
{
	
	if(pID < 0 || pID >= psize)
	{
		printf("\nPTable::JoinUpdate: pID = %d does not exist.\n",pID);
		return -1;
	}

	if (pcb[pID] == NULL)
	{
		printf("PTable::JoinUpdate: no process matches the given pID!\n");
		return -1;
	}

//kiem tra tien trinh dang chay co la cha cua tien trinh can join hay khong
	if(currentThread->processID != pcb[pID]->parentID)
	{
		printf("\nPTable::JoinUpdate: unable to join in a non-parental thread !!!\n");
		return -1;
	}
/////////////////////////////////////////////////////////////////////////////////////////////
	pcb[ pcb[pID]->parentID ]->IncNumWait(); // increase numwait of its parent process

	pcb[pID]->JoinWait(); 	//doi den khi tien trinh con ket thuc

	int ec = pcb[pID]->GetExitCode();

	if(ec != 0)
	{
		printf("\nProcess exit with exitcode EC = %d ",ec);
		return -1;
	}

	pcb[pID]->ExitRelease();	//cho phep tien trinh con ket thuc
	
	// success
	return ec; // equivalent to return 0;
}

void PTable::Remove(int pID)
{
	if(pID<0 || pID>9)
		return;
	if(bm->Test(pID))
	{
		bm->Clear(pID);
		delete pcb[pID];
	}
}

//----------------------------------------------------------------------------------------------
int PTable::GetFreeSlot()
{
	return bm->FindFreeSlot();
}

bool PTable::IsExist(int pID)
{
	if(pID<0 || pID>9)
		return 0;
	return bm->Test(pID);
}

char* PTable::GetName(int pID)
{
	if(pID>=0 && pID<10 && bm->Test(pID))
		return pcb[pID]->GetNameThread();
}
