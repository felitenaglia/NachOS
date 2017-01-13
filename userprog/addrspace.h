// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "noff.h"

#define UserStackSize		1024 	// increase this as necessary!

#ifdef VM
enum PageStatus {SWAPPED, MEMORY, BINARY};
#endif


class AddrSpace {
  public:
    AddrSpace(OpenFile *_executable, int pid);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 
    void WriteArgs(int argc, char** argv);

#ifdef USE_TLB
    bool TakeFromTLB(int virtualPage);   
#endif

#ifdef VM
    void ValidatePage (int virtualPage);
    void Evict (int vpn);
#endif

    void SetNotUsed (int virtualPage);
    bool IsUsed (int virtualPage);
    bool IsDirty (int virtualPage);

 private:
#ifdef VM
    void LoadPageFromBinary(int virtualPage);
    void LoadPageFromSwap(int virtualPage);
    OpenFile *swap;
    char swapName[128];
    PageStatus *pageStatus;
#endif
    NoffHeader noffH;
    OpenFile *executable;
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!

    unsigned int numPages;		// Number of pages in the virtual 
					// address space
};

#endif // ADDRSPACE_H
