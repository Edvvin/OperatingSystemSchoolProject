#ifndef _pcb_h_
#define _pcb_h_
#include "thread.h"
#include <stdlib.h>

#define MAX_SIGNAL_COUNT 16

class SleepQueue;
class Thread;
class KernelSem;
class SignalHandlerList;
class SignalQueue;

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
    static volatile unsigned lockFlag;
    static volatile unsigned contextSwitchFlag;
    static volatile unsigned signalFlag;
    unsigned sp;
    unsigned ss;
    unsigned bp;

    static unsigned dispatchFlag;
    KernelSem* sem;
    static KSemList* kSemList;
    static PCBList* pcbList;

    static void wrapper();
    static void finalize();
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
    static SleepQueue sleepQ;
    SignalHandlerList* (regs[MAX_SIGNAL_COUNT]);
    static unsigned globBlocked[MAX_SIGNAL_COUNT];
    unsigned blocked[MAX_SIGNAL_COUNT];
    SignalQueue *signalQueue;
    PCB* parent;

    void signal(SignalId signal);
    void systemSignal(SignalId signal);
    void registerHandler(SignalId signal, SignalHandler handler);
    void unregisterAllHandlers(SignalId id);
    void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
    void blockSignal(SignalId signal);
    static void blockSignalGlobally(SignalId signal);
    void unblockSignal(SignalId signal);
    static void unblockSignalGlobally(SignalId signal);
    static void handle();
    static void signal0();
    static void initSystem();
    static void restoreSystem();
};
#endif