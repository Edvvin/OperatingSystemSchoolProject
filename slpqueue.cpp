#include "slpqueue.h"
#include "pcb.h"
#include "semqueue.h"
#include "SCHEDULE.h"

SleepQueue::SleepQueue(){
    first = NULL;
}

SleepQueue::~SleepQueue(){
	SleepQueueElement *t = first;
	while(t != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	first = NULL;
}

//TODO: prekontrolisi
void SleepQueue::add(SleepQueueElement* newElem){
    SleepQueueElement* temp = first;
    SleepQueueElement* pred = NULL;
    if(!first){
        first = newElem;
    }else{
        while(temp){
            if(newElem->time >= temp->time){
                pred = temp;
                newElem->time -= temp->time;
                temp = temp->next;
            }else{
                newElem->next = temp;
                temp->time -= newElem->time;
                if(pred == NULL){
                    first = newElem;
                }else{
                    pred->next = newElem;
                }
                break;
            }
        }
        if(!temp){
            pred->next = newElem;
            newElem->next = NULL;
        }
    }
}

void SleepQueue::awaken(){
    if(!first)
        return;
    first->time--;
    while(first->time == 0){
        SleepQueueElement* temp = first;
        first = first->next;
        if(temp->valid){
            temp->sem->value++;
            temp->pair->valid = 0;
            temp->val->status = READY;
            temp->val->signaled = 0;
            Scheduler::put(temp->val);
        }
        delete temp;
        if(!first)
            return;
    }
}