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
    return myPCB->getId();
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

void Thread::signal(SignalId signal){
    myPCB->signal(signal);
}

void Thread::registerHandler(SignalId signal, SignalHandler handler){
    myPCB->registerHandler(signal,handler);
}

void Thread::unregisterAllHandlers(SignalID id){
    myPCB->unregisterAllHandlers(id);
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
    myPCB->swap(id,hand1,hand2);
}

void Thread::blockSignal(SignalId signal){
    myPCB->blockSignal(signal);
}

void Thread::blockSignalGlobally(SignalId signal){
    PCB::blockSignalGlobally(signal);
}

void Thread::unblockSignal(SignalId signal){
    myPCB->unblockSignal(signal);
}

void Thread::unblockSignalGlobally(SignalId signal){
    PCB::unblockSignalGlobally(signal);
}