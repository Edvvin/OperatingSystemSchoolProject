#ifndef _kev_h_
#define _kev_h_
#include "event.h"
class KernelEv
{
  public:
    KernelEv(IVTNo ivtNo, Event* myEv);
    ~KernelEv();
    void wait();
    void signal();
  private:
    Event *myEvent;
};
#endif