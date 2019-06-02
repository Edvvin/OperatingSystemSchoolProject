#ifndef _signalq_h_
#define _signalq_h_

#include "pcb.h"

class PCB;

class SignalQueue{
	struct Element{
		SignalId signal;
		Element *next;
	};


public:
	Element *first, *end;
	SignalQueue();
	~SignalQueue();

	void put(SignalId signal);
	SignalId get();
    SignalId peek();
    unsigned isEmpty();

    struct Iterator{
        private:
            SignalQueue *queue;
            Element *value;
            Element *prev;
        public:
            Iterator(SignalQueue *);
            void remove();
            SignalId get();
            void next();
            unsigned end();
    };

    Iterator getIterator();
};

#endif
