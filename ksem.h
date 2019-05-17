#ifndef _ksem_h_
#define _ksem_h_
#include "semaphor.h"

class KernelSem
{
  public:
    KernelSem(int init, Semaphore* myS);
    ~KernelSem();
    int wait(Time maxTimeToWait);
    int signal(int n);
    int val() const;
  private:
    Semaphore *mySem;
};
#endif