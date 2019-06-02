#include "ivtentry.h"
#include <dos.h>
#include <stdlib.h>
#include "kev.h"
#include <assert.h>

IVTEntry* IVTEntry::entries[IVT_SIZE];

IVTEntry::IVTEntry(IVTNo ivtNo, pointerInterrupt newInterrupt):ev(NULL){
    entries[ivtNo] = (IVTEntry*)this;
    oldInterrupt = getvect(ivtNo);
	setvect(ivtNo, newInterrupt);
}

IVTEntry::~IVTEntry(){
    setvect(ivtNo, oldInterrupt);
    //old();
    //TODO: proveri je l treba delete ev ili old();
}

void IVTEntry::old(){
    (*oldInterrupt)();
}


void IVTEntry::signal(){
    if(ev){
        ev->signal();
    }
}

void IVTEntry::regEvent(KernelEv* ev){
    if(entries[ev->ivtNo])
        entries[ev->ivtNo]->ev = ev;
}
