#include "slpqueue.h"
#include "pcb.h"
#include "semqueue.h"
#include "SCHEDULE.h"
#include "ksem.h"

SleepQueue::SleepQueue(){
    first = NULL;
}

SleepQueue::~SleepQueue(){
	Element *t = first;
	while(first != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	first = NULL;
}

//TODO: prekontrolisi
void SleepQueue::add(PCB* thread, Time t){
    Element* newElem = new Element();
    newElem->val = thread;
    newElem->time = t;
    Element* temp = first;
    Element* pred = NULL;
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

unsigned SleepQueue::awaken(){
    if(!first)
        return 0;
    first->time--;
    unsigned cnt = 0;
    while(first->time == 0){
        Element* temp = first;
        first = first->next;
        temp->val->status = READY;
        temp->val->signaled = 0;
        Scheduler::put(temp->val);
        cnt++;
        delete temp;
        if(!first)
            break;
    }
    return cnt;
}

unsigned SleepQueue::awakenAll(){
    Element *temp = first;
    unsigned cnt = 0;
	while(first != NULL){
		temp = first;
		first = first->next;
        temp->val->status = READY;
        temp->val->signaled = 0;
        Scheduler::put(temp->val);
        cnt++;
        delete temp;
	}
	return cnt;
}

PCB* SleepQueue::getFirst(){
    if(first){
        PCB* ret = first->val;
        if(first->next){
            first->next->time += first->time;
        }
        first = first->next;
        return ret;
    }
    return NULL;
}