#include "ksem.h"
#include "semqueue.h"
#include "sleepqueue.h"

int KernelSem::wait(Time maxTimeToWait){
    if(--value<0){
        PCB::running->signaled = 1;
		block(maxTimeToWait);
	}
    return PCB::running->signaled;
}

void KernelSem::block(Time maxTimeToWait){
    QueueElement* qe = new QueueElement();
    qe->val = PCB::running;
    qe->valid = 1;
    qe->pair = NULL;
    SleepQueueElement* sqe = NULL;
    if(maxTimeToWait){
        sqe = new SleepQueueElement();
        sqe->sem = this;
        sqe->val = PCB::running;
        sqe->time = maxTimeToWait;
        sqe->valid = 1;
        sqe->pair = qe;
        qe->pair = sqe;
    }
    //lock
    PCB::running->status = BLOCKED;
    if(maxTimeToWait)
        KernelSem::sleepQ.add(sqe);
    semq.put(qe);
    dispatch();
    //unlock
}

int KernelSem::signal(int n){
    if(n<0)
        return n;
    if(n>0){
        int i = 0;
        while(value < 0 && i < n){
            signal(0);
            i++;
        }
        value += n - i;
        return i;
    }
    //lock
	if(value++<0){
        QueueElement* e = semq.get();
        while(!e->valid){
            delete e;
            QueueElement* e = semq.get();
        }
        if(e->pair)
            e->pair->valid = 0;
        e->val->status = READY;
        Scheduler::put(e->val);
        delete e;
	}
    //unlock
    return 0;
}