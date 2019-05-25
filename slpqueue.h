#include "thread.h"
#include "pcb.h"

class SemQueue;
class QueueElement;

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