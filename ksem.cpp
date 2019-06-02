#include "ksem.h"
#include "slpqueue.h"
#include "SCHEDULE.H"
#include "ksemlist.h"
#include <assert.h>
#include "lock.h"
#include "pcblst.h"
#include "pcb.h"

KernelSem::KernelSem(int init, Semaphore* myS): value(init), mySem(myS){
    PCB::kSemList.put((KernelSem*)this);
}
KernelSem::~KernelSem(){
    if(value<0)signal(-value);
    KSemList::Iterator it = PCB::kSemList.getIterator();
    while(!it.end()){
        if(it.get() == this){
            it.remove();
            break;
        }
        it.next();
    }
}
int KernelSem::wait(Time maxTimeToWait){
    assert(PCB::lockFlag == 0);
    lock
    if(--value<0){
        PCB::running->signaled = 1;
		block(maxTimeToWait);
        unlockDispatch
        return PCB::running->signaled;
	}
    unlock
    return 1;
}

void KernelSem::block(Time maxTimeToWait){
    PCB::running->status = BLOCKED;
    if(maxTimeToWait){
        sleepQ.add(PCB::running,maxTimeToWait);
    }else{
        semq.put(PCB::running);
    }
}

int KernelSem::signal(int n){
    if(n<0)
        return n;
    int cnt = 0;
    if(n > 0){
        lock
		PCB::inSemSignal = 1;
        while(cnt < n && value<0){
            value++;
            cnt++;
            PCB* thread = semq.get();
            if(!thread){
                thread = sleepQ.getFirst();
            }
            thread->status = READY;
            Scheduler::put(thread);
        }
        value += n-cnt;
        PCB::inSemSignal = 0;
        unlock
    }else{
        lock
		PCB::inSemSignal = 1;
        if(value++<0){
            PCB* thread = semq.get();
            if(!thread){
                thread = sleepQ.getFirst();
            }
            thread->status = READY;
            Scheduler::put(thread);
        }
        PCB::inSemSignal = 0;
        unlock
    }
    return cnt;
}


int KernelSem::val() const{
    return value;
}


/*
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
        PCB::sleepQ.add(sqe);
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
            e = semq.get();
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
*/
