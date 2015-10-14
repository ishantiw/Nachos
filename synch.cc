/*! \file synch.cc 
//  \brief Routines for synchronizing threads.  
//
//      Three kinds of synchronization routines are defined here: 
//      semaphores, locks and condition variables.
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation. We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts. While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
*/
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.


#include "kernel/system.h"
#include "kernel/scheduler.h"
#include "kernel/synch.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
/*! 	Initializes a semaphore, so that it can be used for synchronization.
//
// \param debugName is an arbitrary name, useful for debugging only.
// \param initialValue is the initial value of the semaphore.
*/
//----------------------------------------------------------------------
Semaphore::Semaphore(char* debugName, int initialValue)
{
  name = new char[strlen(debugName)+1];
  strcpy(name,debugName);
  value = initialValue;
  queue = new Listint;
  typeId = SEMAPHORE_TYPE_ID;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
/*! 	De-allocates a semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
*/
//----------------------------------------------------------------------
Semaphore::~Semaphore()
{
  typeId = INVALID_TYPE_ID;
  ASSERT(queue->IsEmpty());
  delete [] name;
  delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
/*!
//      Decrement the value, and wait if it becomes < 0. Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
*/
//----------------------------------------------------------------------
void
Semaphore::P() {
	#ifndef ETUDIANTS_TP
  	printf("**** Warning: method Semaphore::P is not implemented yet\n");
  	exit(-1);
	#endif

	#ifdef ETUDIANTS_TP
	IntStatus oldLevel = g_machine->interrupt->SetStatus(INTERRUPTS_OFF); //disable interrupts to make it atomic 

 	while (value == 0) {
	   queue->Append((void *)g_current_thread);  // semaphore is not available and the thread is blocked (sleep)
	   g_current_thread->Sleep();
	   	
	   }
        value--;
 	g_machine->interrupt->SetStatus(oldLevel); // enable interrupts 
	#endif
		}

//----------------------------------------------------------------------
// Semaphore::V
/*! 	Increment semaphore value, waking up a waiting thread if any.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that interrupts
//	are disabled when it is called.
*/
//----------------------------------------------------------------------
void
Semaphore::V() {
	
	#ifndef ETUDIANTS_TP
   	printf("**** Warning: method Semaphore::V is not implemented yet\n");
    	exit(-1);
	#endif

	#ifdef ETUDIANTS_TP
	Thread *thread;
	IntStatus oldLevel = g_machine->interrupt->SetStatus(INTERRUPTS_OFF); //disable interrupts to make it atomic 

	thread = (Thread *)queue->Remove();


	if(thread!=NULL)
	   g_scheduler->ReadyToRun(thread);
        value++;
	g_machine->interrupt->SetStatus(oldLevel); // enable interrupts 
  	#endif
}

//----------------------------------------------------------------------
// Lock::Lock
/*! 	Initialize a Lock, so that it can be used for synchronization.
//      The lock is initialy free
//  \param "debugName" is an arbitrary name, useful for debugging.
*/
//----------------------------------------------------------------------
Lock::Lock(char* debugName) {
  name = new char[strlen(debugName)+1];
  strcpy(name,debugName);
  sleepqueue = new Listint;
  free = true;
  owner = NULL;
  typeId = LOCK_TYPE_ID;
}


//----------------------------------------------------------------------
// Lock::~Lock
/*! 	De-allocate lock, when no longer needed. Assumes that no thread
//      is waiting on the lock.
*/
//----------------------------------------------------------------------
Lock::~Lock() {
  typeId = INVALID_TYPE_ID;
  ASSERT(sleepqueue->IsEmpty());
  delete [] name;
  delete sleepqueue;
}

//----------------------------------------------------------------------
// Lock::Acquire
/*! 	Wait until the lock become free.  Checking the
//	state of the lock (free or busy) and modify it must be done
//	atomically, so we need to disable interrupts before checking
//	the value of free.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
*/
//----------------------------------------------------------------------
void Lock::Acquire() {

	#ifndef ETUDIANTS_TP
   	printf("**** Warning: method Lock::Acquire is not implemented yet\n");
    	exit(-1);
	#endif

	#ifdef ETUDIANTS_TP
        IntStatus oldLevel = g_machine->interrupt->SetStatus(INTERRUPTS_OFF); // disable interrupts
 	if(g_current_thread == owner) 
	{
    		g_machine->interrupt->SetStatus(oldLevel); // enable interrupts
    		return;
        }
  	if(free) {
    	free = false;                             // lock is no longer free
    	owner = g_current_thread;                // now owned by currentThread
  	}
	else
	{
	    sleepqueue->Append((void *)g_current_thread); // add to lock wait queue
	    g_current_thread->Sleep();                   // put to sleep
  	}
   	g_machine->interrupt->SetStatus(oldLevel);     // enable interrupts
	#endif
}

//----------------------------------------------------------------------
// Lock::Release
/*! 	Wake up a waiter if necessary, or release it if no thread is waiting.
//      We check that the lock is held by the g_current_thread.
//	As with Acquire, this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
*/
//----------------------------------------------------------------------
void Lock::Release() {
	#ifndef ETUDIANTS_TP
    	printf("**** Warning: method Lock::Release is not implemented yet\n");
   	exit(-1);
	#endif
	#ifdef ETUDIANTS_TP
      	Thread *thread;
  	IntStatus oldLevel = g_machine->interrupt->SetStatus(INTERRUPTS_OFF); // disable interrupts
  	if(g_current_thread!= owner) {
    	if(owner!= NULL)
	{
      		printf ("Error: Thread (%s) is not the lock owner (%s)!\n", g_current_thread->GetName(), owner->GetName()); // print error message
    	}
	else
	{
	       printf("Error: There is no lock owner; thread (%s) cannot release the lock!\n", g_current_thread->GetName());
    	}
    	g_machine->interrupt->SetStatus(oldLevel);            // restore interrupts
    	return;
  	}
  	if(!(sleepqueue->IsEmpty())) {
    	thread = (Thread *)sleepqueue->Remove(); // remove a thread from lock's wait queue
    	if(thread != NULL) {
      	g_scheduler->ReadyToRun(thread);    // put in ready queue in ready state
      	owner = thread;               // make lock owner
    	}
  	}
	else
	{
		free = true;                      // make lock free
    		owner = NULL;                   // clear lock ownership
  	}
  	g_machine->interrupt->SetStatus(oldLevel); // restore interrupts
  	#endif
}

//----------------------------------------------------------------------
// Lock::isHeldByCurrentThread
/*! To check if current thread hold the lock
*/
//----------------------------------------------------------------------
bool Lock::isHeldByCurrentThread() {return (g_current_thread == owner);}	

//----------------------------------------------------------------------
// Condition::Condition
/*! 	Initializes a Condition, so that it can be used for synchronization.
//
//    \param  "debugName" is an arbitrary name, useful for debugging.
*/
//----------------------------------------------------------------------
Condition::Condition(char* debugName) { 
  name = new char[strlen(debugName)+1];
  strcpy(name,debugName);
  waitqueue = new Listint;
  typeId = CONDITION_TYPE_ID;
}

//----------------------------------------------------------------------
// Condition::~Condition
/*! 	De-allocate condition, when no longer needed.
//      Assumes that nobody is waiting on the condition.
*/
//----------------------------------------------------------------------
Condition::~Condition() {
  typeId = INVALID_TYPE_ID;
  ASSERT(waitqueue->IsEmpty());
  delete [] name;
  delete waitqueue;
}

//----------------------------------------------------------------------
// Condition::Wait
/*! Block the calling thread (put it in the wait queue).
//  This operation must be atomic, so we need to disable interrupts.
*/	
//----------------------------------------------------------------------
void Condition::Wait() { 
	#ifndef ETUDIANTS_TP
   	printf("**** Warning: method Condition::Wait is not implemented yet\n");
    	exit(-1);
	#endif
	#ifdef ETUDIANTS_TP
	IntStatus oldLevel = g_machine->interrupt->SetStatus(INTERRUPTS_OFF); //disable interrupts

	waitqueue->Append((void*)g_current_thread); //add the thread in the list
	g_current_thread->Sleep(); //wait for a signal

    	g_machine->interrupt->SetStatus(oldLevel); //enable interrupts
	#endif
}

//----------------------------------------------------------------------
// Condition::Signal

/*! Wake up the first thread of the wait queue (if any). 
// This operation must be atomic, so we need to disable interrupts.
*/
//----------------------------------------------------------------------
void Condition::Signal() { 
	#ifndef ETUDIANTS_TP
    	printf("**** Warning: method Condition::Signal is not implemented yet\n");
    	exit(-1);
	#endif
	#ifdef ETUDIANTS_TP
	IntStatus oldLevel = g_machine->interrupt->SetStatus(INTERRUPTS_OFF); //disable interrupts
  	Thread* thread = (Thread*) waitqueue->Remove();
	if(thread != NULL)
	{
		g_scheduler->ReadyToRun(thread);
	}
	else
	{
	  printf("Error: No threads to signal");
	}
    	g_machine->interrupt->SetStatus(oldLevel); //enable interrupts
	#endif
}

//----------------------------------------------------------------------
/*! Condition::Broadcast
// wake up all threads waiting in the waitqueue of the condition
// This operation must be atomic, so we need to disable interrupts.
*/
//----------------------------------------------------------------------
void Condition::Broadcast() { 
  #ifndef ETUDIANTS_TP
  printf("**** Warning: method Condition::Broadcast is not implemented yet\n");
  exit(-1);
  #endif
  #ifdef ETUDIANTS_TP
  IntStatus oldLevel = g_machine->interrupt->SetStatus(INTERRUPTS_OFF); //disable interrupts to make it atomic 
	Thread* thread;
	while((thread=(Thread*)waitqueue->Remove()) != NULL)
	{
		g_scheduler->ReadyToRun(thread); //Wake up all the threads
	}
	g_machine->interrupt->SetStatus(oldLevel); //enable interrupts
}
  #endif

