#include "pcb.h"
#include <dos.h>
#include <stdlib.h>

extern void tick();

ID PCB::PID = 0;

PCB* volatile PCB::running = NULL;

unsigned PCB::dispatchFlag = 0;

ID PCB::getId(){
    return pid;
}

ID PCB::getRunningId(){
    return running->pid;
}

Thread *getThreadById(ID id){ // TODO: TREBA PROMENITI NA VEKTOR
    if(id < 0){
        return NULL;
    }
    if(threads[id] == NULL){
        return NULL; // Mozda niz threads nije inicijalizovan na 0
    }
    return threads[id];
}

PCB::PCB(StackSize stacksize, Time timeslice, Thread* myThr):sem(0, NULL){
    stackSize = (stacksize + 1)/2; // stackSize promenjiva broji reci a ne bajtove
    timeSlice = timeslice;
    myThread = myThr;
    pid = PID++;
    threads[pid] = myThr; // TODO: TREBA PROMENITI NA VEKTOR
    status = CREATED;
	stack = NULL;
    remaining = timeSlice;
}

void PCB::start(){
    if(status == CREATED){
        createThread();
        Scheduler::put(this);
        status = READY;
    }
}


unsigned PCB::noTimeSlice(){
    return !timeSlice;
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

void PCB::initIdle(){
    PCB::idle = new PCB(32,1, NULL);
    idle->stack = new unsigned[idle->stackSize];

	idle->stack[idle->stackSize-1] = 0x200; //PSW, setovan I flag

	idle->stack[idle->stackSize-2] = FP_SEG(PCB::idleRun); //PC
	idle->stack[idle->stackSize-3] = FP_OFF(PCB::idleRun);
    // push ax,bx,cx,dx,es,ds,si,di,bp
	idle->bp = idle->sp = FP_OFF(idle->stack + idle->stackSize-12); 
	idle->ss = FP_SEG(idle->stack + idle->stackSize-12);
    idle->status = IDLE;
}

void PCB::initMain(){
    running = new PCB(0, 0, NULL);
}

void PCB::initTimer(){
    // TODO:
}

void PCB::restoreTimer(){
    // TODO: 
}

void PCB::killIdle(){
    // TODO: 
}

void PCB::killMain(){
    // TODO: 
}

void PCB::wrapper(){
    running->myThread->run();
    if(running->sem.val() > 0)
        running->sem.signal(-(running->sem.val()));
    running->status = COMPLETED;
    dispatch(); // TODO: Proveri je l samo ovo?
}

void PCB::idleRun(){
    while(1);
}


void PCB::waitToComplete(){
    if(status == COMPLETED)
        return;
    sem.wait(0);
}

void PCB::dispatch(){
    dispatchFlag = 1;
    myTimer(); 
}


// TODO: remove comment
void /*interrupt*/ myTimer(...){
    if(!PCB::dispatchFlag){
        tick();
        //(*PCB::oldTimer)(); TODO: 
        // TODO: Semafor sleepqueue
        if(PCB::running->noTimeSlice())
            return;
        
        PCB::running->remaining--;
        // TODO: Mozda ovde lockFlag?
        if(PCB::running->remaining > 0)
            return;
        // TODO: azuriraj contextSwitchFlag
        PCB::running->remaining = PCB::running->timeSlice;
    //Timer stuff
    }
    
    PCB::tsp = _SP;
    PCB::tss = _SS;
    PCB::tbp = _BP;
    PCB::running->sp = PCB::tsp;
    PCB::running->ss = PCB::tss;
    PCB::running->bp = PCB::tbp;

    if(PCB::running->status == READY){
        Scheduler::put(PCB::running);
    }

    PCB::running = Scheduler::get();

    if(PCB::running == NULL)
        PCB::running = PCB::idle; // TODO: napravi idle
    
    PCB::tbp = PCB::running->bp;
    PCB::tss = PCB::running->ss;
    PCB::tsp = PCB::running->sp;
    _BP = PCB::tbp;
    _SS = PCB::tss;
    _SP = PCB::tsp;

    PCB::dispatchFlag = 0;
}