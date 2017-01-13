#ifndef COREMAP_H
#define COREMAP_H

#include "system.h"
#include "utility.h"
#include "bitmap.h"
#include "dlist.h"
#include <unistd.h>

class CoreMap {
	public:
	CoreMap() {
		bmpMem = new BitMap(NumPhysPages);
		owner = new AddrSpace*[NumPhysPages];
		vpn = new int[NumPhysPages];
		fifoQueue = new DoubleList<int>;
		posPages = new DListNode<int>*[NumPhysPages];
		for (int i=0; i<NumPhysPages ; i++) {
			owner[i]  = NULL;
			posPages[i] = NULL;
		}
	}
	~CoreMap() {
		delete bmpMem;
		delete[] owner;
		delete[] vpn;
		delete fifoQueue;
		delete[] posPages;
	}
	void SetFreePage(int physPage){
		ASSERT(posPages[physPage]!=NULL);
		fifoQueue->Remove(posPages[physPage]);
		posPages[physPage] = NULL;
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

		posPages[freePage] = fifoQueue->Append(freePage);
		owner[freePage] = who;
		vpn[freePage] = virtPage;

		return freePage;
	}
	void OneUse(int physPage) {

		ASSERT(physPage>=0 && physPage<NumPhysPages);
		ASSERT(posPages[physPage]!=NULL);
		fifoQueue->Remove(posPages[physPage]);
		posPages[physPage] = fifoQueue->Append(physPage);
	}
	
	private:
		BitMap *bmpMem;
		AddrSpace **owner;
		int *vpn;
		DoubleList<int> *fifoQueue;
		DListNode<int>** posPages;
		int NextToEvict() {
			stats->numEvicts++;
			ASSERT(fifoQueue->Begin()!=NULL);

			#ifdef USE_TLB
			currentThread->space->SaveState(); // Puede que el bit haya sido seteado en used, pero la modificación no haya llegado al espacio de direcciones por estar en la TLB
			#endif

			DListNode<int>* it = fifoQueue->Begin();
			while (owner[it->item]->IsUsed(vpn[it->item])) {
				owner[it->item]->SetNotUsed(vpn[it->item]);
				int physPage = fifoQueue->Remove(fifoQueue->Begin());
				posPages[physPage] = fifoQueue->Append(physPage);
				it = fifoQueue->Begin();
				//it= it->next!=NULL ? it->next : fifoQueue->Begin();
			}
			return fifoQueue->Remove(it);
		}
};

#endif
