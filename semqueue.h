#ifndef _semqueue_h_
#define _semqueue_h_

#include "pcb.h"

class PCB;



class SemQueue{
	struct Element{
		PCB *val;
		Element *next;
	};
	Element *first, *end;
public:
	SemQueue();
	~SemQueue();
	void put(PCB* thread);
	PCB* get();
};

#endif
