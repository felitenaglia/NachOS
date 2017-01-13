#include "port.h"

Port::Port(const char* debugName) {
    name = debugName;
}

/*
   Comprobamos que no haya emisores esperando
   Si hay receptores esperando, estarán bloqueados por la variable de condición de la lista sincronizada y por lo tanto fallará la destrucción de la misma
*/
Port::~Port() 
{
	ASSERT(portList.IsEmpty());
} 

void 
Port::Send(int msg) 
{
	PortElement newPortElement;
	newPortElement.data = msg;
	newPortElement.sem = new Semaphore(NULL,0);
	portList.Append(newPortElement);
	newPortElement.sem->P();
	delete newPortElement.sem;
}

void 
Port::Receive(int *msg) 
{
	PortElement recv = portList.Remove();
	recv.sem->V();
	if (msg!=NULL)
		*msg = recv.data;
}
