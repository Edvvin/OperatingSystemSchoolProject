#include "pcblst.h"
#include <stdlib.h>

PCBList::PCBList(){
	end = first= NULL;
}

PCBList::~PCBList(){
	Element *t = first;
	while(first != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	end = first = NULL;
}

void PCBList::put(PCB* thread){
	if(!first){
		end = first = new Element();
	}
	else{
		end = end->next = new Element();
	}
    end->val = thread;
	end->next = NULL;
}

PCBList::Iterator PCBList::getIterator(){
    return PCBList::Iterator(this);
}

PCBList::Iterator::Iterator(PCBList* sq){
    queue = sq;
    prev = NULL;
    value = sq->first;
}

void PCBList::Iterator::remove(){
    Element *t = value;
    if (t == NULL)
        return;
    if (prev != NULL) {
        prev->next = value->next;
    }else{
    	queue->first = value->next;
    }
    value = value->next;
    if (value == NULL)
    	queue->end = prev;
    delete t;
}

PCB* PCBList::Iterator::get(){
    if(value == NULL)
        return NULL;
    else 
        return value->val;
}

void PCBList::Iterator::next(){
    if (value != NULL){
        prev = value;
        value = value->next;
    }
}

unsigned PCBList::Iterator::end(){
    return value == NULL;
}
