#ifndef _pcb_h
#define _pcb_h_
#include "thread.h"
#define MAX_PROCESS_COUNT 1000 //TREBA PROMENITI NA VEKTOR

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
  private:
    void wrapper();
    void createThread();
    Thread *myThread;
    ID pid;

    StackSize stackSize;
    Time timeSlice;
    unsigned *stack;
    STATUS status;
    unsigned started;
    unsigned completed;
    static ID PID;
    static PCB* running;
    static Thread* (threads[MAX_PROCESS_COUNT]); //TREBA PROMENITI NA VEKTOR
};
#endif