#ifndef _ksem_h_
#define _ksem_h_
#include "semaphor.h"
#include "thread.h"
#include "pcb.h"


class KernelSem
{
  public:
    KernelSem(int init, Semaphore* myS): value(init), mySem(myS){}
    ~KernelSem(){if(value<0)signal(-value);}
    int wait(Time maxTimeToWait);
    int signal(int n);
    int val() const;
    void block(Time maxTimeToWait);
    Semaphore *mySem;
    int value;
    SemQueue semq;
};
#endif