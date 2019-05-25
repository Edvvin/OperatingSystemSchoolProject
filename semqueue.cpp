#include "semqueue.h"
#include "pcb.h"
#include <stdlib.h>

SemQueue::SemQueue(){
	first = end = NULL;
}

SemQueue::~SemQueue(){
	QueueElement *t = first;
	while(t != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	first = end = NULL;
}

void SemQueue::put(QueueElement *qe){
	qe->next = NULL;
	if(first == NULL){
		first = end = qe;
	}
	else{
		end = end->next = qe;
	}
}

QueueElement* SemQueue::get(){
	if(first == NULL)
		return NULL;
	QueueElement *p = first;
	first = p->next;
	p->next = NULL;
	return p;
}
