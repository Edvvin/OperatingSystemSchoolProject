#ifndef _slpqueue_h_
#define _slpqueue_h_
#include "thread.h"
#include "pcb.h"

class SemQueue;
class QueueElement;

class SleepQueue{
  struct Element{
    Time time;
    PCB* val;
    Element* next;
  };
  public:
    SleepQueue();
    ~SleepQueue();
    void add(PCB* thread, Time t);
    unsigned awaken();
    unsigned awakenAll();
    PCB* getFirst();
  private:
    Element *first;
};
#endif
