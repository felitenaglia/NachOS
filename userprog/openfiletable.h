#ifndef OPENFILETABLE_H
#define OPENFILETABLE_H

#include "openfile.h"
#include "syscall.h"

#define MAX_FILE 16

class OpenFileTable {
	public:
	OpenFileTable(){
		int pos;
		for (pos = 0 ; pos < MAX_FILE ; pos++)
			fileTable[pos]=NULL;

	}
	~OpenFileTable(){
		int pos;
		for (pos = 0 ; pos < MAX_FILE ; pos++)
			if(fileTable[pos]!=NULL)
				delete fileTable[pos];

	}

	OpenFileId NewEntry(OpenFile *toInsert) {
		int pos;
		for (pos = 0 ; pos < MAX_FILE ; pos++)
			if (fileTable[pos]==NULL){
				fileTable[pos] = toInsert;
				return pos+2;
			}
		delete toInsert;
 		return -1;	
	}
	bool DeleteEntry(OpenFileId entry) {
		if(entry >= 2 && entry < MAX_FILE + 2 && fileTable[entry-2]!=NULL){
			delete fileTable[entry-2];
			fileTable[entry-2]=NULL;
			return true;
		}else{
			return false;
		}
	}
	OpenFile* Index(OpenFileId entry) {
		if(entry >= 2 && entry < MAX_FILE + 2)
			return fileTable[entry-2];
		else
			return NULL;
	}

	private:
	OpenFile* fileTable[MAX_FILE];
};

#endif

