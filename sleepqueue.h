#include "thread.h"
#include "pcb.h"

class SemQueue;

#ifndef NULL
#define NULL 0
#endif

struct SleepQueueElement{
    Time time;
    PCB* val;
    unsigned valid;
    QueueElement* pair;
    KernelSem* sem;
    SleepQueueElement* next;
};


class SleepQueue{
  public:
    SleepQueue();
    ~SleepQueue();
    void add(SleepQueueElement* newElem);
    void awaken();
  private:
    SleepQueueElement* first;
};