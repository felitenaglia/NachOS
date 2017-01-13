#ifndef PROCESSESTABLE_H
#define PROCESSESTABLE_H

#include "thread.h"
#include "syscall.h"

#define MAX_PROC 16

class ProcessesTable {
	public:
	ProcessesTable(){
		int pos;
		for (pos = 0 ; pos < MAX_PROC ; pos++)
			procTable[pos]=NULL;

	}

	SpaceId NewEntry(Thread *toInsert) {
		int pos;
		for (pos = 0 ; pos < MAX_PROC ; pos++)
			if (procTable[pos]==NULL){
				procTable[pos] = toInsert;
				return pos;
			}
 		return -1;	
	}
	bool DeleteEntry(SpaceId entry) {
		if(entry >= 0 && entry < MAX_PROC && procTable[entry]!=NULL){
			procTable[entry]=NULL;
			return true;
		}else{
			return false;
		}
	}
	Thread* Index(SpaceId entry) {
		if(entry >= 0 && entry < MAX_PROC)
			return procTable[entry];
		else
			return NULL;
	}

	private:
	Thread* procTable[MAX_FILE];
};

#endif

