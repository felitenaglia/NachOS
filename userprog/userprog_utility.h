#ifndef USERPROG_UTILITY
#define USERPROG_UTILITY

#define THROW(predicado, exc) if(!(predicado)) throw exc;

#if defined(USE_TLB) || defined(VM)
    #define READ(usrAddr,size,pointer) { if ( !machine->ReadMem(usrAddr,size,pointer) )               \
                                            THROW(machine->ReadMem(usrAddr,size,pointer),ExcReadMem)  \
                                       }
    #define WRITE(usrAddr,size,value) {  if ( !machine->WriteMem(usrAddr,size,value)  )               \
                                            THROW(machine->WriteMem(usrAddr,size,value),ExcWriteMem)  \
                                      }
#else
    #define READ(usrAddr,size,pointer) THROW(machine->ReadMem(usrAddr,size,pointer),ExcReadMem)
    #define WRITE(usrAddr,size,value) THROW(machine->WriteMem(usrAddr,size,value),ExcWriteMem)
#endif

enum RTExceptions {ExcReadMem, ExcWriteMem, ExcJoin, ExcBadFD, ExcWrtIn, ExcReaOut};


void readStrFromUsr(int usrAddr, char *outStr);

void readBuffFromUsr(int usrAddr, char *outBuff, int byteCount);

void writeStrToUsr(char *str, int usrAddr);

void writeBuffToUsr(char *str, int usrAddr, int byteCount);

#endif
