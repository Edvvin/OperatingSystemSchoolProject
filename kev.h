#ifndef _kev_h_
#define _kev_h_
#include "event.h"

class KernelSem;
class PCB;
class KernelEv
{
  public:
    KernelEv(IVTNo ivtNo, Event* myEv);
    ~KernelEv();
    void wait();
    void signal();
    Event *myEvent;
    KernelSem* sem;
    PCB* maker;
    IVTNo ivtNo;
};
#endif