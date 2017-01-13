#include "userprog_utility.h"
#include "system.h"

void 
readStrFromUsr(int usrAddr, char *outStr)
{
	ASSERT(outStr!=NULL);
	int aux;
	do {
		READ(usrAddr, 1, &aux);
		*outStr=aux;
		outStr++;
		usrAddr++;
	}  while(aux);
}

void 
readBuffFromUsr(int usrAddr, char *outBuff, int byteCount)
{
	ASSERT(outBuff!=NULL);
	int aux;
	for(int i=0;i<byteCount;i++){
		READ(usrAddr+i, 1, &aux);
		outBuff[i]=aux;
	}
}

void 
writeStrToUsr(char *str, int usrAddr)
{
	ASSERT(str!=NULL);
	int i;
	for (i=0;str[i];i++)
		WRITE(usrAddr+i, 1,(int)str[i]);
	WRITE(usrAddr+i, 1,0);
}

void 
writeBuffToUsr(char *str, int usrAddr, int byteCount)
{
	ASSERT(str!=NULL);
	for (int i=0;i<byteCount;i++)
		WRITE(usrAddr+i, 1,(int)str[i]);
}
