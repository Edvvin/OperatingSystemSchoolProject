#include "thread.h"
#include "pcb.h"

Thread::Thread(StackSize stackSize, Time timeSlice){
    myPCB = new PCB(stackSize, timeSlice, this);
}

void Thread::start(){
    myPCB->start();
}

void Thread::waitToComplete(){
    myPCB->waitToComplete();
}

Thread::~Thread(){
    waitToComplete();
    delete myPCB;
}
ID Thread::getId(){
    return myPCB->getID();
}

ID Thread::getRunningId(){
    return PCB::getRunningId();
}

Thread* Thread::getThreadById(ID id){
    return PCB::getThreadById(id);
}

void dispatch(){
    PCB::dispatch();
}