#ifndef _sighandl_h_
#define _sighandl_h_
#include "pcb.h"


class SignalHandlerList{
    struct Element{
        SignalHandler sh;
        Element *next;
    };
	Element *first, *end;
public:
	SignalHandlerList();
	~SignalHandlerList();
	void reg(SignalHandler handler);
	void unreg();
    void invoke();
    void swap(SignalHandler hand1, SignalHandler hand2);
    SignalHandlerList* copy() const;
};

#endif
