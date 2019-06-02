#include "pcb.h"
#include <dos.h>
#include <stdlib.h>
#include "SCHEDULE.h"
#include "slpqueue.h"
#include "ksem.h"
#include "sighandl.h"
#include "signalq.h"
#include "lock.h"
#include "ksemlist.h"
#include "pcblst.h"
#include <iostream.h>

extern void tick();


ID PCB::PID = 0;

PCB* volatile PCB::running = NULL;
Thread* PCB::mainPCBWrapper = NULL;
PCB* PCB::idle = NULL;

unsigned PCB::dispatchFlag = 0;

const IVTNo PCB::IVTNo_TIMER = 0x08;

pointerInterrupt PCB::oldTimer = NULL;

unsigned PCB::globBlocked[MAX_SIGNAL_COUNT];

KSemList PCB::kSemList;
PCBList* PCB::pcbList = NULL;

volatile unsigned PCB::lockFlag = 0;
volatile unsigned PCB::contextSwitchFlag = 0;
volatile unsigned PCB::signalFlag = 0;
volatile unsigned PCB::inSemSignal = 0;
void interrupt myTimer(...);
void interrupt killer(...);

ID PCB::getId(){
    return pid;
}

ID PCB::getRunningId(){
    return running->pid;
}

Thread* PCB::getThreadById(ID id){
    PCBList::Iterator it = PCB::pcbList->getIterator();
    PCB* ret = NULL;
    while(!it.end()){
        if(it.get()->pid == id){
            ret = it.get();
            break;
        }
        it.next();
    }
    if(ret){
        return ret->myThread;
    }
    return NULL;
}

PCB::PCB(StackSize stacksize, Time timeslice, Thread* myThr){
    sem = new KernelSem(0,0);
    stackSize = (stacksize + 1)/2; // stackSize promenjiva broji reci a ne bajtove
    timeSlice = timeslice;
    myThread = myThr;
    pid = PID++;
    status = CREATED;
	stack = NULL;
    remaining = timeSlice;
    signaled = 1;
    parent = PCB::running;
    for(int i = 0;i < MAX_SIGNAL_COUNT; i++){
        if(parent){
            blocked[i] = parent->blocked[i];
            regs[i] = parent->regs[i]->copy();
        }
        else{
            blocked[i] = 0;
            regs[i] = new SignalHandlerList();
        }
    }
    if(!parent)
    	regs[0]->reg(PCB::signal0);
    signalQueue = new SignalQueue();
    PCB::pcbList->put((PCB*)this);
}

PCB::~PCB(){
	if(status == DESTROYED){
		delete sem;
		sem = NULL;
		for(int i = 0;i < MAX_SIGNAL_COUNT; i++){
			delete regs[i];
			regs[i] = NULL;
		}
		delete signalQueue;
		signalQueue = NULL;
		PCBList::Iterator it = PCB::pcbList->getIterator();
		while(!it.end()){
			if(it.get() == this){
				it.remove();
				break;
			}
			it.next();
		}

		delete [] stack;
		stack = NULL;
	}else{
	    delete sem;
	    for(int i = 0;i < MAX_SIGNAL_COUNT; i++){
	        delete regs[i];
	        regs[i] = NULL;
	    }
	    delete signalQueue;
	    signalQueue = NULL;
		PCBList::Iterator it = PCB::pcbList->getIterator();
		while(!it.end()){
			if(it.get() == this){
				it.remove();
				break;
			}
			it.next();
		}
	    delete [] stack;
	    stack = NULL;
	}
}

void PCB::start(){
    if(status == CREATED){
        createThread();
        status = READY;
        Scheduler::put(this);
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
	mainPCBWrapper = new Thread(0,0);
    running = mainPCBWrapper->myPCB;
    running->status = READY;
}

void PCB::initTimer(){
    oldTimer = getvect(IVTNo_TIMER);
	setvect(IVTNo_TIMER, myTimer);
}

void PCB::restoreTimer(){
    setvect(IVTNo_TIMER, oldTimer);
}

void PCB::killIdle(){
    delete idle;
    idle = NULL;
}

void PCB::killMain(){
    delete mainPCBWrapper;
    mainPCBWrapper = NULL;
    running = NULL;
}

void PCB::wrapper(){
    running->myThread->run();
    finalize();
}

void PCB::signal0(){
    if(!PCB::running->parent){
        return;
    }
    if(running->sem->val() < 0)
        running->sem->signal(-(running->sem->val()));
    running->status = DESTROYED;
    if(PCB::running->parent){
        PCB::running->parent->systemSignal(1);
    }
    PCB::running->systemSignal(2);
}

void PCB::finalize(){
    if(running->sem->val() < 0)
        running->sem->signal(-(running->sem->val()));
    running->status = COMPLETED;
    if(PCB::running->parent)
        PCB::running->parent->systemSignal(1);
    PCB::running->systemSignal(2);
    dispatch();
}

void PCB::idleRun(){
    while(1);
}


void PCB::waitToComplete(){
    if(this == PCB::running || status == CREATED || status == COMPLETED || status == DESTROYED)
        return;
    sem->wait(0);
}

void PCB::dispatch(){
    dispatchFlag = 1;
    myTimer(); 
}

void interrupt myTimer(...){
    if(!PCB::dispatchFlag){
    	(*PCB::oldTimer)();
    	tick();
        if(!PCB::inSemSignal){
			KSemList::Iterator it = PCB::kSemList.getIterator();
			while(!it.end()){
				KernelSem* ksem = it.get();
				unsigned cnt = ksem->sleepQ.awaken();
				ksem->value += cnt;
				it.next();
			}
        }
        if(PCB::running->noTimeSlice())
            return;
        PCB::running->remaining--;
        if(PCB::running->remaining > 0)
            return;
        PCB::running->remaining = PCB::running->timeSlice;
    //Timer stuff
    }
    if(PCB::lockFlag){
        PCB::contextSwitchFlag = 1;
        return;
    }
    PCB::running->sp = _SP;
    PCB::running->ss = _SS;
    PCB::running->bp = _BP;

    if(PCB::running->status == READY){
        Scheduler::put(PCB::running);
    }
    do{
    	PCB::running = Scheduler::get();
    }while(PCB::running != NULL && PCB::running->status == DESTROYED);

    if(PCB::running == NULL)
        PCB::running = PCB::idle;

    _BP = PCB::running->bp;
    _SS = PCB::running->ss;
    _SP = PCB::running->sp;
     
    PCB::running->remaining = PCB::running->timeSlice;
    PCB::dispatchFlag = 0;

    PCB::lockFlag++;
    //asm sti;
    PCB::handle();
    //asm cli;
    PCB::lockFlag--;

    if (PCB::running->status == DESTROYED) {
    	PCB::running->sp = _SP;
		PCB::running->ss = _SS;
		PCB::running->bp = _BP;

		do{
			PCB::running = Scheduler::get();
		}while(PCB::running != NULL && PCB::running->status == DESTROYED);

		if(PCB::running == NULL)
			PCB::running = PCB::idle;

		_BP = PCB::running->bp;
		_SS = PCB::running->ss;
		_SP = PCB::running->sp;

		PCB::running->remaining = PCB::running->timeSlice;
    }
}


void PCB::signal(SignalId signal){
    if(signal >= MAX_SIGNAL_COUNT)
        return;
    if(signal == 1 || signal == 2)
        return;
    signalQueue->put(signal);
}

void PCB::systemSignal(SignalId signal){
    signalQueue->put(signal);
}

void PCB::registerHandler(SignalId signal, SignalHandler handler){
	if(signal >= MAX_SIGNAL_COUNT) return;
	if(signal == 0) return;
    regs[signal]->reg(handler);
}

void PCB::unregisterAllHandlers(SignalId id){
	if(id >= MAX_SIGNAL_COUNT) return;
    if(id == 0)
        return;
    regs[id]->unreg();
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
	if(id >= MAX_SIGNAL_COUNT) return;
    regs[id]->swap(hand1,hand2);
}

void PCB::blockSignal(SignalId signal){
	if(signal >= MAX_SIGNAL_COUNT) return;
    blocked[signal] = 1;
}

void PCB::blockSignalGlobally(SignalId signal){
	if(signal >= MAX_SIGNAL_COUNT) return;
    globBlocked[signal] = 1;
}

void PCB::unblockSignal(SignalId signal){
	if(signal >= MAX_SIGNAL_COUNT) return;
    blocked[signal] = 0;
}

void PCB::unblockSignalGlobally(SignalId signal){
	if(signal >= MAX_SIGNAL_COUNT) return;
    globBlocked[signal] = 0;
}

void PCB::handle(){
    PCB::signalFlag = 1;
    SignalQueue::Iterator it = PCB::running->signalQueue->getIterator();
    while(!it.end()) {
        SignalId signal = it.get();
        if(!PCB::globBlocked[signal] && !PCB::running->blocked[signal]){
            PCB::running->regs[signal]->invoke();
            it.remove();
        }else{
            it.next();
        }
    }
    PCB::signalFlag = 0;
}

void PCB::initSystem(){
    PCB::pcbList = new PCBList();
    PCB::initMain();
    PCB::initIdle();

    PCB::initTimer();
}

void PCB::restoreSystem(){
    PCB::restoreTimer();

    // TODO: IVTEntry restore
    PCB::killIdle();
    PCB::killMain();
    //delete pcbList;
    //delete kSemList;

}
