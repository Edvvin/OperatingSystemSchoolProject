#include "sighandl.h"
#include <stdlib.h>

SignalHandlerList::SignalHandlerList(){
	end = first = NULL;
}

SignalHandlerList::~SignalHandlerList(){
    unreg();
}

void SignalHandlerList::reg(SignalHandler handler){
	if(!first){
		end = first = new Element();
	}
	else{
		end = end->next = new Element();
	}
    end->sh = handler;
	end->next = NULL;
}

void SignalHandlerList::unreg(){
	Element *t = first;
	while(t != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	first = end = NULL;
}


void SignalHandlerList::invoke(){
    Element* t = first;
    while(t != NULL){
    	asm sti;
        (t->sh)();
        asm cli;
        t = t->next;
    }
}

void SignalHandlerList::swap(SignalHandler hand1, SignalHandler hand2){
    Element *el1 = NULL, *el2 = NULL;
    Element* t = first;
    if(hand1 == hand2)
        return;
    while(t != NULL){
        if(t->sh == hand1 || t->sh == hand2){
            el1 = t;
            t = t->next;
            break;
        }
        t = t->next;
    }
    if(el1 == NULL)
        return;
    while(t != NULL){
        if((t->sh == hand1 || t->sh == hand2) && t->sh != el1->sh){
            el2 = t;
            break;
        }
        t = t->next;
    }
    if(el2 == NULL)
        return;
    SignalHandler temp = el1->sh;
    el1->sh = el2->sh;
    el2->sh = temp;
}

SignalHandlerList* SignalHandlerList::copy() const{
    SignalHandlerList* rez = new SignalHandlerList();
    Element* t = first;
    while(t != NULL){
        rez->reg(t->sh);
        t = t->next;
    }
    return rez;
}
