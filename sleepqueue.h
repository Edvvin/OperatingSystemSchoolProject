#include "thread.h"
#include "pcb.h"

class SemQueue;

#ifndef NULL
#define NULL 0
#endif


class SleepQueue{
  struct Elem{
    Time time;
    PCB* waitingThread;
    Elem* next;
    Elem(PCB* waitingThread, Time t)
    :time(t),next(NULL),waitingThread(waitingThread){}
  };
  public:
    SleepQueue();
    ~SleepQueue();
    void addThread(PCB* waitingThread, Time t);
    void decrease();
    PCB* get();
  private:
    Elem* first;
    SemQueue semq;
};