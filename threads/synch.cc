// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List<Thread*>;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append(currentThread);		// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    interrupt->SetLevel(oldLevel);		// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(const char* debugName) :
    sem(debugName,1)
{
    name = debugName;
    owner = NULL;
}

Lock::~Lock() { 
	ASSERT(processList.IsEmpty());
} 

void auxiliar_max_prio(Thread *thread, void *maxContainer) {
     if ( *(int*)maxContainer < thread->GetPriority()  ) 
	*(int*)maxContainer = thread->GetPriority();
}

void Lock::Acquire() {
    ASSERT(!isHeldByCurrentThread());

    // Se asume que los cambios de contexto solo ocurren cuando prendemos/apagamos interrupciones o se hace un P, V de semáforo
    if (owner!=NULL && owner->GetPriority() < currentThread->GetPriority()) { // Si el lock está tomado, significa que currentThread dormirá en P.
        int oldPriority = owner->GetPriority();                               // Si la prioridad del dueño es menor que la de currentThread,
	owner->SetPriority(currentThread->GetPriority());                     // debo hacer que el dueño herede la prioridad
	scheduler->Update(owner,oldPriority);                                 // el dueño posiblemente esté READY, debo cambiarlo de cola a la nueva prioridad en el scheduler
    }
    processList.Append(currentThread); // El nuevo hilo pasa a la cola de los que requieren el thread
    sem.P(); // Intenta hacerse del semáforo de valor 1, si lo consigue es porque no había dueño
    ASSERT(owner==NULL);  // Por las dudas hacemos un ASSERT para verificar aquello
    int maxLeft = 0;                                               // Si se hace con el lock (posiblemente despertó de P) debemos hacer que herede la máxima
    processList.ApplyWithContext(auxiliar_max_prio,&maxLeft);      // prioridad de los procesos bloqueados esperando el lock como no sacamos todavía al
    currentThread->SetPriority(maxLeft);                           // proceso de la lista, nos aseguramos que la herencia sólo aumente la prioridad del proceso
    processList.RemoveOnce(currentThread);                         // removemos el proceso de la lista de procesos ques esperan por el lock
    owner = currentThread;                                         // ahora currentThread es el nuevo dueño del Lock
}

bool Lock::isHeldByCurrentThread() {
    return currentThread==owner;
}	
void Lock::Release() {
    ASSERT(isHeldByCurrentThread());
    owner = NULL;
    currentThread->RestorePriority();   // Si se libera el Lock el proceso dueño restablece su prioridad (leer readme)
    sem.V();
}

Condition::Condition(const char* debugName, Lock* conditionLock) {
    condLock = conditionLock;
    name = debugName;
}

Condition::~Condition() {
	ASSERT(waitingQueue.IsEmpty()); // Pedimos que la cola esté vacía porque no sería correcto destruirse una variable de condición si hay hilos esperando
}

void Condition::Wait() {
    Semaphore *ns = new Semaphore(NULL, 0); 
    waitingQueue.Append(ns);
    condLock->Release();
    ns->P();
    delete ns;
    condLock->Acquire();
}

void Condition::Signal() {
    Semaphore *popSem;
    if (!waitingQueue.IsEmpty())
    {
        popSem = waitingQueue.Remove();
        popSem->V();
    }
}

void Condition::Broadcast() {
    Semaphore *popSem;
    while (!waitingQueue.IsEmpty())
    {
        popSem = waitingQueue.Remove();
        popSem->V();       
    }
}

