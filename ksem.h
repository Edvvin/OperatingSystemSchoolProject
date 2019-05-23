#ifndef _ksem_h_
#define _ksem_h_
#include "semaphor.h"
#include "thread.h"
#include "pcb.h"

#ifndef NULL
#define NULL 0
#endif

class KernelSem
{
  public:
    KernelSem(int init, Semaphore* myS);
    ~KernelSem();
    int wait(Time maxTimeToWait);
    int signal(int n);
    int val() const;
    Semaphore *mySem;
    friend class SleepQueue;
    int value;
};
#endif