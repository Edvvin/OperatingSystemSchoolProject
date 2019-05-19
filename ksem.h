#ifndef _ksem_h_
#define _ksem_h_
#include "semaphor.h"

class KernelSem
{
  public:
    KernelSem(int init, Semaphore* myS);
    ~KernelSem();
    int wait(Time maxTimeToWait) volatile;
    int signal(int n) volatile;
    int val() const volatile;
  private:
    Semaphore *mySem;
};
#endif