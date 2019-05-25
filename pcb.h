#ifndef _pcb_h
#define _pcb_h
#include "thread.h"
#include <stdlib.h>

#define MAX_PROCESS_COUNT 1000 // TODO: TREBA PROMENITI NA VEKTOR


class SleepQueue;
class Thread;
class KernelSem;

typedef unsigned char IVTNo;

typedef void interrupt(*pointerInterrupt)(...); // Mozda u IVTEntry

enum STATUS{READY, COMPLETED, BLOCKED, SLEEP, CREATED, IDLE};

class PCB
{
  public:
    void start();
    void waitToComplete();
    virtual ~PCB();
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

    static unsigned dispatchFlag;
    KernelSem* sem;

    static void wrapper();
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
    volatile unsigned signaled;
    static pointerInterrupt oldTimer;
    static const IVTNo IVTNo_TIMER; // Mozda premestiti u IVTEntry
    static ID PID;
    static Thread* (threads[MAX_PROCESS_COUNT]); //TREBA PROMENITI NA VEKTOR
    static SleepQueue sleepQ;
};
#endif