#ifndef _pcb_h
#define _pcb_h_
#include "thread.h"
#include "ksem.h"
#define MAX_PROCESS_COUNT 1000 // TODO: TREBA PROMENITI NA VEKTOR


enum STATUS{READY, COMPLETED, BLOCKED, SLEEP, CREATED, IDLE};

class PCB
{
  public:
    void start();
    void waitToComplete();
    virtual ~PCB() {}
    ID getId();
    static ID getRunningId();
    static Thread *getThreadById(ID id);
    static void dispatch();
    PCB(StackSize stacksize, Time timeslice, Thread* myThr);

    static PCB* volatile running;
    static PCB* idle;
    unsigned sp;
    unsigned ss;
    unsigned bp;

    static unsigned tsp;
    static unsigned tss;
    static unsigned tbp;
    static unsigned dispatchFlag;
    KernelSem sem;

    void wrapper();
    void createThread();
    unsigned noTimeSlice();
    static void initIdle();
    static void idleRun();
    static void initMain();
    static void initTimer();
    static void restoreTimer();
    static void killIdle();
    static void killMain();
    Thread *myThread;
    ID pid;
    StackSize stackSize;
    Time remaining;
    Time timeSlice;
    unsigned *stack;
    volatile STATUS status;
    static ID PID;
    static Thread* (threads[MAX_PROCESS_COUNT]); //TREBA PROMENITI NA VEKTOR
};
#endif