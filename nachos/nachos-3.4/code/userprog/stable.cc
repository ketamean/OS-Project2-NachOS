#include "stable.h"
// FOR WHATEVER REASON THE UP AND DOWN FUNCTIONS ARE NAMED SIGNAL AND WAIT
// Constructor
STable::STable()
{	
	this->bm = new BitMap(MAX_SEMAPHORE);
	
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		this->semTab[i] = NULL;
	}
}

// Destructor
STable::~STable()
{
	if(this->bm)
	{
		delete this->bm;
		this->bm = NULL;
	}
	for(int i=0; i < MAX_SEMAPHORE; i++)
	{
		if(this->semTab[i])
		{
			delete this->semTab[i];
			this->semTab[i] = NULL;
		}
	}
	
}

int STable::Create(char *name, int init)
{

	// If the semaphore name already exists, return -1
	for(int i=0; i<MAX_SEMAPHORE; i++)
	{
		if(bm->Test(i))
		{
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				return -1;
			}
		}
		
	}
	// Find a free slot
	int id = this->FindFreeSlot();
	
	// If there is no free slot, return -1
	if(id < 0)
	{
		return -1;
	}

	// Create a new semaphore with name and initial value
	this->semTab[id] = new Sem(name, init);
	return 0;
}

int STable::Wait(char *name) // This is Down() operation
{
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		// Check if the i-th slot has been loaded with a semaphore
		if(bm->Test(i))
		{
			// If yes, compare the name with the name of the semaphore in semTab
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				// If it exists, execute the semaphore wait();
				semTab[i]->wait();
				return 0;
			}
		}
	}
	printf("Semaphore does not exist");
	return -1;
}

int STable::Signal(char *name) // This is Up() operation
{
    // Similar to the Wait function
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		// Check if the i-th slot has been loaded with a semaphore
		if(bm->Test(i))
		{
			// If yes, compare the name with the name of the semaphore in semTab
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				// If it exists, execute the semaphore signal();
				semTab[i]->signal();
				return 0;
			}
		}
	}
	printf("Semaphore does not exist");
	return -1;
}

int STable::FindFreeSlot()
{
	return this->bm->FindFreeSlot();
}
