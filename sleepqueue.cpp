#include "sleepqueue.h"
#include "pcb.h"
#include "semqueue.h"

SleepQueue::SleepQueue(){
    first = NULL;
}

SleepQueue::~SleepQueue(){
	Elem *t = first;
	while(t != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	first = NULL;
}

//TODO: prekontrolisi
void SleepQueue::addThread(PCB* waitingThread, Time t){
    Elem* newElem = new Elem(waitingThread,t);
    Elem* temp = first;
    Elem* pred = NULL;
    if(!first){
        first = newElem;
    }
    else{
        while(temp){
            if(newElem->time > temp->time){
                pred = temp;
                newElem->time -= temp->time;
                temp = temp->next;
            }
            else{
                newElem->next = temp;
                temp->time -= newElem->time;
                if(pred == 0){
                first = newElem;
            }else{
                pred->next = newElem;
            }
            break;
            }
        }
        if(!temp){
            pred->next = newElem;
        }
    }
}

void SleepQueue::decrease(){
    if(!first)
        return;
    first->time--;
    while(first->time == 0){
        Elem* temp = first;
        first = first->next;
        semq.insert(temp->waitingThread);
        delete temp;
        if(!first)
            return;
    }
}

PCB* SleepQueue::get(){
    return semq.remove();
}