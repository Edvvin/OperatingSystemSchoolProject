#include "event.h"
#include "kev.h"

Event::Event(IVTNo ivtNo){
    myImpl = new KernelEv(ivtNo,this);
}

Event::~Event(){
    delete myImpl;
}

void Event::wait(){
    myImpl->wait();
}

void Event::signal(){
    myImpl->signal();
}