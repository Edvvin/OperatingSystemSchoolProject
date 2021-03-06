#ifndef _ksemlist_h_
#define _ksemlist_h_
#include <stdlib.h>
#include "pcb.h"

class PCB;

class KSemList{
	struct Element{
		KernelSem* ksem;
		Element *next;
		Element(){
			next = NULL;
		}
	};
public:
	Element *first, *end;
	KSemList();
	~KSemList();
    void put(KernelSem* ksem);
    struct Iterator{
        private:
            KSemList *queue;
            Element *value;
            Element *prev;
        public:
            Iterator(KSemList *);
            void remove();
            KernelSem* get();
            void next();
            unsigned end();
    };

    Iterator getIterator();
};

#endif
