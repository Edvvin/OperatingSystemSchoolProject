#include "semqueue.h"
#include "pcb.h"
#include <stdlib.h>

SemQueue::SemQueue(){
	len = 0;
	first = end = NULL;
}

SemQueue::~SemQueue(){
	len = 0;
	Elem *t = first;
	while(t != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	first = end = NULL;
}

void SemQueue::insert(PCB *pcb){
	if(len++ == 0){
		first = end = new Elem();
	}
	else{
		end = end->next = new Elem();
	}
	end->val = pcb;
	end->next = NULL;
}

PCB* SemQueue::remove(){
	if(len == 0)
		return NULL;
	len--;
	PCB *p = first->val;
	Elem *t = first;
	first = first->next;
	delete t;
	return p;
}

int SemQueue::size() const{
	return len;
}

