#ifndef _ksem_h_
#define _ksem_h_
#include "semaphor.h"
#include "semqueue.h"
#include "slpqueue.h"


class KernelSem
{
  public:
    KernelSem(int init, Semaphore* myS);
    ~KernelSem();
    int wait(Time maxTimeToWait);
    int signal(int n);
    int val() const;
    void block(Time maxTimeToWait);
    Semaphore *mySem;
    int value;
    SemQueue semq;
    SleepQueue sleepQ;
};
#endif