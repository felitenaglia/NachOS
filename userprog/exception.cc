// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include <unistd.h>
#include "system.h"
#include "syscall.h"

#include "openfiletable.h"
#include "userprog_utility.h"

static void scExit();

#define RUNTIMEERR(str) { \
		fprintf(stderr, "Error en tiempo de ejecución: %s\n", str); \
		machine->WriteRegister(4,-1); \
		scExit(); \
		ASSERT(false); }

#define INCPC() { int pc = machine->ReadRegister(PCReg); \
		machine->WriteRegister(PrevPCReg,pc); \
		pc = machine->ReadRegister(NextPCReg); \
		machine->WriteRegister(PCReg,pc); \
		pc+=4; \
		machine->WriteRegister(NextPCReg,pc); }

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

struct argumentos {
	int argc;
	char **argv;
};

static void scHalt () {
	DEBUG('a', "Shutdown, initiated by user program.\n");
	interrupt->Halt();
}

static void auxForkExec(void* args) {
	argumentos *args_exec = (argumentos*) args;
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	try{
		currentThread->space->WriteArgs(args_exec->argc, args_exec->argv);

		for (int i = 0; i<args_exec->argc;i++)
			delete[] args_exec->argv[i];

		delete[] args_exec->argv;
		delete args_exec;
		machine->Run();
		ASSERT(false);
	}catch(...){
		for (int i = 0; i<args_exec->argc;i++)
			delete[] args_exec->argv[i];

		delete[] args_exec->argv;
		delete args_exec;
	}
}

static void scExec () {
	char name[128];
	int arg;
	int raddr = machine->ReadRegister(4);
	int argc = machine->ReadRegister(5);
	int argv_addr = machine->ReadRegister(6);
	int pid=-1;
	int i;
	OpenFile *binary;
	try{
		readStrFromUsr(raddr,name);

		binary = fileSystem->Open(name);
		if (binary!=NULL){
			Thread *t = new Thread("execthread");
			AddrSpace *addSpace = new AddrSpace(binary,t->GetPid());
			t->space = addSpace;
			pid = t->GetPid();
			char **argv = new char*[argc];
			for (i=0; i<argc; i++)
				argv[i] = new char[MAX_LENGTH_ARG];
			try{
				for (i=0; i<argc; i++){
					READ(argv_addr+i*4,4,&arg);
					readStrFromUsr(arg,argv[i]);
				}
				argumentos *args = new argumentos;
				args->argc = argc;
				args->argv = argv;
				t->Fork(auxForkExec, args);
			}catch(RTExceptions e){
				processTable->DeleteEntry(pid);
				delete t;
				for (i=0; i<argc;i++)
					delete[] argv[i];
				delete[] argv;
				RUNTIMEERR("Lectura de memoria inválida");
			}
		}
	}catch(RTExceptions e){
		RUNTIMEERR("Lectura de memoria inválida");
	}catch(int e){
		pid=-2;	// Para diferenciar si hubo un error porque el binario no existe (pid = -1) o si hubo un error porque no hay mas memoria (pid = -2)
	}
	machine->WriteRegister(2,pid);
	INCPC();
}

static void scExit () {
	int ret = machine->ReadRegister(4);
	currentThread->Finish(ret);
	ASSERT(false);
}


static void scJoin () {
	int pid = machine->ReadRegister(4);
	int retValue;
	try{
		Thread* thread = processTable->Index(pid);
		THROW(thread!=NULL, ExcJoin);
		retValue = thread->Join();
	}catch(...){
		RUNTIMEERR("Join en un SpaceId inválido");
	}
	machine->WriteRegister(2,retValue);
	INCPC();
}

static void scCreate () {
	int raddr = machine->ReadRegister(4);
	char name[128];
	try{
		readStrFromUsr(raddr, name);
		fileSystem->Create(name,1);
	}catch(...){
		RUNTIMEERR("Lectura de memoria inválida");
	}
	INCPC();
}

static void scOpen () {
	int raddr = machine->ReadRegister(4);
	char name[128];
	int entryId = -1;
	OpenFile* archivo;
	
	try{
		readStrFromUsr(raddr, name);
		archivo = fileSystem->Open(name); 
		if(archivo) 
		     entryId = currentThread->fileTable->NewEntry(archivo);
	}catch(...){
		RUNTIMEERR("Lectura de memoria inválida");
	}
	machine->WriteRegister(2, entryId);

	INCPC();

}

static void scRead () {
	int raddr = machine->ReadRegister(4);
	int size  = machine->ReadRegister(5);
	int openFileId = machine->ReadRegister(6);
	int i;
	int numRead = 0;
	try{
		THROW(openFileId!=ConsoleOutput, ExcReaOut);
		char buff[size];

		if (openFileId==ConsoleInput) {
			for (i=0; i<size; i++)
				buff[i] = synchConsole->GetChar();
			numRead = size;
		} else {
			OpenFile *file = currentThread->fileTable->Index(openFileId);
			THROW(file!=NULL,ExcBadFD);
			numRead = file->Read(buff,size);
		}
		writeBuffToUsr(buff,raddr,numRead);
	}catch(RTExceptions e){
		switch (e) {
			case ExcReaOut: RUNTIMEERR("Intentando leer de ConsoleOutput"); break;
			case ExcBadFD: RUNTIMEERR("Descriptor de archivo inválido"); break;
			case ExcWriteMem: RUNTIMEERR("Escritura en memoria inválida"); break;
			default: RUNTIMEERR("Excepcion no esperada");
		}
	}
	machine->WriteRegister(2, numRead);
	INCPC();
}

static void scWrite () {
	int raddr = machine->ReadRegister(4);
	int size  = machine->ReadRegister(5);
	int openFileId = machine->ReadRegister(6);
	int i;

	try{
		THROW(openFileId!=ConsoleInput, ExcWrtIn);

		char buff[size];
		readBuffFromUsr(raddr,buff,size);
	
		if (openFileId==ConsoleOutput) {
			for (i=0; i<size; i++)
				synchConsole->PutChar(buff[i]);
		} else {
			OpenFile *file = currentThread->fileTable->Index(openFileId);
			THROW(file!=NULL, ExcBadFD);
			file->Write(buff,size);
		}
	}catch (RTExceptions e){
		switch (e) {
			case ExcWrtIn: RUNTIMEERR("Intentando escribir en ConsoleInput"); break;
			case ExcBadFD: RUNTIMEERR("Descriptor de archivo inválido"); break;
			case ExcReadMem: RUNTIMEERR("Lectura de memoria inválida"); break;
			default: RUNTIMEERR("Excepcion no esperada");
		}
	}
	INCPC();
}

static void scClose () {
	int openFileId = machine->ReadRegister(4);
	currentThread->fileTable->DeleteEntry(openFileId);

	INCPC();
}

static void scFork () {
	ASSERT(false);
}


static void scYield () {
	ASSERT(false);
}


void
ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	int badAddr,vpn;

	switch (which) {
		case SyscallException:
			switch (type) {
				case SC_Halt:
					scHalt();
					break;
				case SC_Exit:
					scExit();
					break;
				case SC_Exec:
					scExec();
					break;
				case SC_Join:
					scJoin();
					break;
				case SC_Create:
					scCreate();
					break;
				case SC_Open:
					scOpen();
					break;
				case SC_Read:
					scRead();
					break;
				case SC_Write:
					scWrite();
					break;
				case SC_Close:
					scClose();
					break;
				case SC_Fork:
					scFork();
					break;
				case SC_Yield:
					scYield();
					break;
			}
			break;
		case PageFaultException:
		#ifdef USE_TLB
			badAddr = machine->ReadRegister(BadVAddrReg);
			vpn = badAddr / PageSize;
			
			if (!currentThread->space->TakeFromTLB(vpn))
				RUNTIMEERR("Segmentation Fault Exception\n");
		#elif defined(VM)
			badAddr = machine->ReadRegister(BadVAddrReg);
			vpn = badAddr / PageSize;
			currentThread->space->ValidatePage(vpn);
		#else
			RUNTIMEERR("Segmentation Fault Exception\n");
		#endif
			break;
		case ReadOnlyException:
			RUNTIMEERR("Read Only Exception\n");
		break;
		default:
			printf("Unexpected user mode exception %d %d\n", which, type);
			RUNTIMEERR("Unexpected exception");
	}
}






