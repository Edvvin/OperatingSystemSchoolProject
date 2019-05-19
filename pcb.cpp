#include "pcb.h"
#include <dos.h>
#include <stdlib.h>

ID PCB::PID = 0;

volatile PCB* PCB::running = NULL;

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
}

void PCB::start(){
    if(status == CREATED){
        createThread();
        Scheduler::put(this);
        status = READY;
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
    if(PCB::running->sem.val() > 0)
        PCB::running->sem.signal(-(PCB::running->sem.val()));
    PCB::running->status = COMPLETED;
    // TODO: dispatch neki nesto !!!
}


/*void PCB::wrapper(){
	Kernel::running->myThread->run();
	lock
	Kernel::running->sem.signalAll();
	delete []Kernel::running->stack;
	Kernel::running->stack = new unsigned char[24];//alocira se mnogo manji stek, zbog potrebe funkcije blockedDispatch()
	tsp = FP_OFF(Kernel::running->stack + 23);
	tss = FP_SEG(Kernel::running->stack + 23);
	asm{
		mov sp, tsp // restore sp
		mov ss, tss
	}
	Kernel::running->status = DONE;
	blockedDispatch();//doneDispatch();// treba da se obrise ovaj PCB i da se pusti neki drugi thread
	
}
*/

void PCB::waitToComplete(){
    if(status == COMPLETED)
        return;
    sem.wait(0);
}

void PCB::dispatch(){
    dispatchFlag = 1;
    myTimer();
    PCB::running->sem.signal(0);
}


// TODO: remove comment
void /*interrupt*/ myTimer(...){
    if(!PCB::dispatchFlag){
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