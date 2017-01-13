// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Parts from Copyright (c) 2007-2009 Universidad de Las Palmas de Gran Canaria
//

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include <unistd.h>
#define NTHREADS 6


int buffer[10];
int libre = 0;
Lock l("");
Condition vacio("vacio",&l), lleno("lleno",&l);



void Productor(void *arg) {
     
     while(1) {
          sleep(2);
          l.Acquire();
          while ( libre>=10 ) lleno.Wait();
          printf("Productor %d produciendo\n", *(int*)arg);
          buffer[libre++] = *(int*)arg;
          printf("Libre: %d\n", libre);
          vacio.Signal();
          l.Release();
     }
}

void Consumidor(void* arg) {
     while (1) {
          sleep(2);
          l.Acquire();
          while ( libre<=0 ) vacio.Wait();
          printf("Consumidor %d consumiendo %d\n", *(int*)arg, buffer[--libre]);
          printf("Libre: %d\n", libre);
          lleno.Signal();
          l.Release();
     }
     
}

void ThrowConsumidor (int id) {
	Thread* t = new Thread("consumidor");
	t->Fork(Consumidor,&id);
	currentThread->Yield();

}

void ThrowProductor (int id) {
	Thread* t = new Thread("productor");
	t->Fork(Productor,&id);
	currentThread->Yield();
}

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    for (int i=0 ; i<10 ; i++) {
        ThrowConsumidor(i);
        ThrowProductor(i);
    }
}

