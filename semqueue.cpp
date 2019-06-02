#include "semqueue.h"
#include "pcb.h"
#include <stdlib.h>

SemQueue::SemQueue(){
	first = end = NULL;
}

SemQueue::~SemQueue(){
	Element *t = first;
	while(first != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	first = end = NULL;
}

void SemQueue::put(PCB* thread){
	Element* qe = new Element();
	qe->val = thread;
	qe->next = NULL;
	if(first == NULL){
		first = end = qe;
	}
	else{
		end = end->next = qe;
	}
}

PCB* SemQueue::get(){
	if(first == NULL)
		return NULL;
	Element *p = first;
	first = p->next;
	PCB* ret = p->val;
	delete p;
	return ret;
}
