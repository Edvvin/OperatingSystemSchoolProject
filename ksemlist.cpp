#include "ksemlist.h"
#include "pcb.h"
#include <stdlib.h>

KSemList::KSemList(){
	end = first= NULL;
}

KSemList::~KSemList(){
	Element *t = first;
	while(first != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	end = first = NULL;
}

void KSemList::put(KernelSem* ksem){
	if(!first){
		end = first = new Element();
	}
	else{
		end = end->next = new Element();
	}
    end->ksem = ksem;
	end->next = NULL;
}

KSemList::Iterator KSemList::getIterator(){
    return KSemList::Iterator(this);
}

KSemList::Iterator::Iterator(KSemList* ksl){
    queue = ksl;
    prev = NULL;
    value = ksl->first;
}

void KSemList::Iterator::remove(){
    Element *t = value;
    if (t == NULL)
        return;
    if (prev != NULL) {
        prev->next = value->next;
    }
    value = value->next;
    delete t;
}

KernelSem* KSemList::Iterator::get(){
    if(value == NULL)
        return NULL;
    else 
        return value->ksem;
}

void KSemList::Iterator::next(){
    if (value != NULL){
        prev = value;
        value = value->next;
    }
}

unsigned KSemList::Iterator::end(){
    return value == NULL;
}