#include "kev.h"
#include "ksem.h"
#include "pcb.h"
#include "ivtentry.h"
#include <stdlib.h>

KernelEv::KernelEv(IVTNo ivtNo, Event* myEv):maker(PCB::running),myEvent(myEv),ivtNo(ivtNo){
    sem = new KernelSem(0, NULL);
    IVTEntry::regEvent((KernelEv*)this);
}

KernelEv::~KernelEv(){
    delete sem;
}

void KernelEv::wait(){
    if(PCB::running == maker)
        sem->wait(0);
}

void KernelEv::signal(){
    if(sem->val() == -1)
        sem->signal(0);
}