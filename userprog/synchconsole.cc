#include "synchconsole.h"

static void SynchConsoleReadAvail(void* arg) { ((SynchConsole *)arg)->ReadAvail(); }
static void SynchConsoleWriteDone(void* arg) { ((SynchConsole *)arg)->WriteDone(); }

SynchConsole::SynchConsole(const char *readFile, const char *writeFile)
{
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    console = new Console(readFile, writeFile, SynchConsoleReadAvail, SynchConsoleWriteDone, this);
    readLock = new Lock("read lock");
    writeLock = new Lock("write lock");
}

SynchConsole::~SynchConsole()
{
    delete readAvail;
    delete writeDone;
    delete console;
    delete readLock;
    delete writeLock;
}

void SynchConsole::PutChar(char ch)
{
    writeLock->Acquire();
    console->PutChar(ch);
    writeDone->P();
    writeLock->Release();
}

char SynchConsole::GetChar()
{
    char ch;
    readLock->Acquire();
    readAvail->P();
    ch = console->GetChar();
    readLock->Release();
    return ch;
}
