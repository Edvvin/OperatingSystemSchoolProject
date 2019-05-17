#include "pcb.h"
#include <dos.h>
#include <stdlib.h>

ID PCB::PID = 0;

PCB* PCB::running = NULL;



ID PCB::getId(){
    return pid;
}

ID PCB::getRunningId(){
    return running->pid;
}

Thread *getThreadById(ID id){ // TREBA PROMENITI NA VEKTOR
    if(id < 0){
        return NULL;
    }
    if(threads[id] == NULL){
        return NULL; // Mozda niz threads nije inicijalizovan na 0
    }
    return threads[id];
}

PCB::PCB(StackSize stacksize, Time timeslice, Thread* myThr){
    stackSize = (stacksize + 1)/2; // stackSize promenjiva broji reci a ne bajtove
    timeSlice = timeslice;
    myThread = myThr;
    started = 0;
    completed = 0;
    pid = PID++;
    threads[pid] = myThr; //TREBA PROMENITI NA VEKTOR
    status = CREATED;
	stack = NULL;
}

void PCB::start(){
    if(!started){
        createThread();
        Scheduler::put(this);
        status = READY;
        started = 1;
    }
}

void PCB::createThread(){
	stack = new unsigned[stackSize];

	stack[stackSize-1] = 0x200; //PSW, setovan I flag

	stack[stackSize-2] = FP_SEG(PCB::wrapper); //PC
	stack[stackSize-3] = FP_OFF(PCB::wrapper);
    // push ax,bx,cx,dx,es,ds,si,di,bp
	bp = sp = FP_OFF(stack + stackSize-12); 
	ss = FP_SEG(stack + stackSize-12);
}

void PCB::wrapper(){
    PCB::running->myThread->run();
    //DO OVDE SI STIGO
    
}