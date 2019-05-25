#ifndef _semqueue_h_
#define _semqueue_h_

#include "pcb.h"

class PCB;
struct SleepQueueElement;

struct QueueElement{
	PCB *val;
	unsigned valid;
	SleepQueueElement* pair;
	QueueElement *next;
};

class SemQueue{
	QueueElement *first, *end;
public:
	SemQueue();
	~SemQueue();
	void put(QueueElement *qe);
	QueueElement* get();
};

#endif
