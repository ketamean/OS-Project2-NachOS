#ifndef STABLE_H
#define STABLE_H
#include "synch.h"
#include "bitmap.h"
#define MAX_SEMAPHORE 10 // Follwing the instruction, we can only create 10 semaphores

class Sem
{
private:
	char name[50];		// Ten cua semaphore
	Semaphore* sem;		// Tao semaphore de quan ly
public:
	// Initialize semaphore with name and initial value
    // If the semaphore is not initialized, the value is 0
    // If the semaphore is initialized, the value is i
	Sem(char* na, int i)
	{
		strcpy(this->name, na);
		sem = new Semaphore(this->name, i);
	}

	~Sem()
	{
		if(sem)
			delete sem;
	}

	void wait()
	{
		sem->P();	// Down(sem)
	}

	void signal()
	{
		sem->V();	// Up(sem)
	}
	
	char* GetName()
	{
		return this->name;
	}
};

class STable
{
private:
	BitMap* bm;	// Empty slot management
	Sem* semTab[MAX_SEMAPHORE];
public:
    // Initialize the empty slot management
    // The initial value of the semaphore is null
	STable();		

	~STable();	// Destructor
	int Create(char *name, int init); // Create a semaphore with name and initial value

	// If the semaphore “name” exists, call this->P() to execute. Otherwise, report an error.
	int Wait(char *name);

	// If the semaphore “name” exists, call this->V() to execute. Otherwise, report an error.
	int Signal(char *name);
	
	// Find the empty slot in the semaphore table
	int FindFreeSlot();
};

#endif // STABLE_H