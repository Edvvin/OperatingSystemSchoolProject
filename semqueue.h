#ifndef _semqueue_h_
#define _semqueue_h_

#include "pcb.h"

class PCB;

class SemQueue{
	struct Elem{
		PCB *val;
		Elem *next;
	};

	Elem *first, *end;
	int len;
public:
	SemQueue();
	~SemQueue();

	void insert(PCB *pcb);
	PCB* remove();

	int size() const;
};

#endif
