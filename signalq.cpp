#include "signalq.h"
#include "pcb.h"
#include <stdlib.h>

SignalQueue::SignalQueue(){
	end = first= NULL;
}

SignalQueue::~SignalQueue(){
	Element *t = first;
	while(first != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	end = first = NULL;
}

void SignalQueue::put(SignalId signal){
	if(!first){
		end = first = new Element();
	}
	else{
		end = end->next = new Element();
	}
	end->signal = signal;
	end->next = NULL;
}

SignalId SignalQueue::get(){
	if(!first)
		return MAX_SIGNAL_COUNT;
	SignalId sig = first->signal;
	Element *t = first;
	first = first->next;
    if(first == NULL){
        end = NULL;
    }
	delete t;
	return sig;
}

SignalId SignalQueue::peek(){
	if(!first)
		return MAX_SIGNAL_COUNT;
	SignalId sig = first->signal;
	return sig;
}

unsigned SignalQueue::isEmpty(){
    return first == NULL;
}

SignalQueue::Iterator SignalQueue::getIterator(){
    return SignalQueue::Iterator(this);
}

SignalQueue::Iterator::Iterator(SignalQueue*sq){
    queue = sq;
    prev = NULL;
    value = sq->first;
}

void SignalQueue::Iterator::remove(){
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

SignalId SignalQueue::Iterator::get(){
    if(value == NULL)
        return MAX_SIGNAL_COUNT;
    else 
        return value->signal;
}

void SignalQueue::Iterator::next(){
    if (value != NULL){
        prev = value;
        value = value->next;
    }
}

unsigned SignalQueue::Iterator::end(){
    return value == NULL;
}
