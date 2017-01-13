#ifndef COREMAP_H
#define COREMAP_H

#include "system.h"
#include "utility.h"
#include "bitmap.h"
#include "dlist.h"

class CoreMap {
	public:
	CoreMap() {
		bmpMem = new BitMap(NumPhysPages);
		owner = new AddrSpace*[NumPhysPages];
		vpn = new int[NumPhysPages];
		for (int i=0; i<NumPhysPages ; i++) 
			owner[i]  = NULL;
	}
	~CoreMap() {
		delete bmpMem;
		delete[] owner;
		delete[] vpn;

	}
	void SetFreePage(int physPage){
		bmpMem->Clear(physPage);
		owner[physPage] = NULL;
	}
	int FindFreePage(AddrSpace *who, int virtPage) {
		int freePage;
		if (  (freePage = bmpMem->Find()) == -1  ) {
			int evictPage = NextToEvict();
			ASSERT(owner[evictPage]!=NULL);
			owner[evictPage]->Evict(vpn[evictPage]);
			freePage = evictPage;
		}

		
		owner[freePage] = who;
		vpn[freePage] = virtPage;

		return freePage;
	}
	
	private:
		BitMap *bmpMem;
		AddrSpace **owner;
		int *vpn;
		int NextToEvict() {
			stats->numEvicts++;
			return (Random() % NumPhysPages);
		}
};

#endif
