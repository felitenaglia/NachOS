#ifndef SYNCH_CONSOLE_H
#define SYNCH_CONSOLE_H

#include "synch.h"
#include "console.h"

class SynchConsole {
  public:
    SynchConsole(const char *readFile, const char *writeFile);

    ~SynchConsole();

    void PutChar(char ch);

    char GetChar();

    void ReadAvail() { readAvail->V(); }
    
    void WriteDone() { writeDone->V(); }


  private:
    Console* console;
    Semaphore* readAvail;
    Semaphore* writeDone;
    Lock* writeLock;
    Lock* readLock;
};

#endif
