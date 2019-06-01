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

extern void tick();

SleepQueue PCB::sleepQ;

ID PCB::PID = 0;

PCB* volatile PCB::running = NULL;

PCB* PCB::idle = NULL;

unsigned PCB::dispatchFlag = 0;

const IVTNo PCB::IVTNo_TIMER = 0x08;

pointerInterrupt PCB::oldTimer = NULL;

unsigned PCB::globBlocked[MAX_SIGNAL_COUNT];

KSemList* kSemList = NULL;
PCBList* pcbList = NULL;

volatile unsigned lockFlag = 0;
volatile unsigned contextSwitchFlag = 0;
volatile unsigned signalFlag = 0;
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
    signalQueue = new SignalQueue();
    regs[0]->reg(PCB::signal0);
    PCB::pcbList->put((PCB*)this);
}

PCB::~PCB(){
    delete[] stack;
    delete sem;
    for(int i = 0;i < MAX_SIGNAL_COUNT; i++){
        delete regs[i];
    }
    delete signalQueue;
    PCBList::Iterator it = PCB::pcbList->getIterator();
    while(!it.end()){
        if(it.get() == this){
            it.remove();
            break;
        }
        it.next();
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
    running = new PCB(0, 0, NULL);
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
}

void PCB::killMain(){
    delete running;
}

void PCB::wrapper(){
    running->myThread->run();
    finalize();
}

void PCB::signal0(){
    if(!PCB::running->parent){
        return;
    }
    killer();
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
    if(this == PCB::running || status == CREATED || status == COMPLETED)
        return;
    sem->wait(0);
}

void PCB::dispatch(){
    if(signalFlag)
        return;
    dispatchFlag = 1;
    myTimer(); 
}

void interrupt killer(...){
    PCB::signalFlag = 0;
	PCB::running->stack[PCB::running->stackSize-1] = 0x200; //PSW, setovan I flag
	PCB::running->stack[PCB::running->stackSize-2] = FP_SEG(PCB::finalize); //PC
	PCB::running->stack[PCB::running->stackSize-3] = FP_OFF(PCB::finalize);
    // push ax,bx,cx,dx,es,ds,si,di,bp
	_SP = FP_OFF(stack + PCB::running->stackSize-12);
    _BP = _SP;
	_SS = FP_SEG(stack + PCB::running->stackSize-12);
}

void interrupt myTimer(...){
    if(!PCB::dispatchFlag){
        tick();
        (*PCB::oldTimer)();
        KSemList::Iterator it = PCB::kSemList->getIterator();
        while(!it.end()){
            KernelSem* ksem = it.get();
            unsigned cnt = ksem->sleepQ.awaken();
            ksem->value += cnt;
            it.next();
        }
        PCB::sleepQ.awaken();
        if(PCB::running->noTimeSlice())
            return;
        PCB::running->remaining--;
        if(PCB::running->remaining > 0)
            return;
        PCB::running->remaining = PCB::running->timeSlice;
    //Timer stuff
    }
    if(PCB::lockFlag){
        contextSwitchFlag = 1;
        return;
    }
    PCB::running->sp = _SP;
    PCB::running->ss = _SS;
    PCB::running->bp = _BP;

    if(PCB::running->status == READY){
        Scheduler::put(PCB::running);
    }

    PCB::running = Scheduler::get();

    if(PCB::running == NULL)
        PCB::running = PCB::idle;
    
    _BP = PCB::running->bp;
    _SS = PCB::running->ss;
    _SP = PCB::running->sp;
     
    PCB::dispatchFlag = 0;

    PCB::lockFlag++;
    asm sti;
    PCB::handle();
    asm cli;
    PCB::lockFlag--;
}


void PCB::signal(SignalId signal){
    if(signal >= MAX_SIGNAL_COUNT)
        return;
    if(signal == 1 || signal == 2)
        return;
    signalQueue->put(signal);
    if(PCB::running == this && !signalFlag)
        PCB::handle();
}

void PCB::systemSignal(SignalID signal){
    signalQueue->put(signal);
    if(PCB::running == this  && !signalFlag ) //TODO: PROVERI OVO
        PCB::handle();
}

void PCB::registerHandler(SignalId signal, SignalHandler handler){
    if(signal == 0)
        return;
    regs[signal]->reg(handler);
}

void PCB::unregisterAllHandlers(SignalId id){
    if(id == 0)
        return;
    regs[id]->unreg();
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
    regs[signal]->swap(hand1,hand2);
}

void PCB::blockSignal(SignalId signal){
    blocked[signal] = 1;
}

void PCB::blockSignalGlobally(SignalId signal){
    globBlocked[signal] = 1;
}

void PCB::unblockSignal(SignalId signal){
    blocked[signal] = 0;
    if(PCB::running == this && !signalFlag)
        PCB::handle();
}

void PCB::unblockSignalGlobally(SignalId signal){
    globBlocked[signal] = 0;
    if(PCB::running == this && !signalFlag)
        PCB::handle();
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
    PCB::kSemList = new KSemList();
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
    KSemList::Iterator it = kSemList->getIterator();
    while(!it.end()){
        delete it.get();
        it.remove();
    }
    delete kSemList;
    PCBList::Iterator it = pcbList->getIterator();
    while(!it.end()){
        delete it.get();
        it.remove();
    }
    delete pcbList;
}