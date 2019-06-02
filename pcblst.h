#ifndef _pcblst_h_
#define _pcblst_h_

#include "pcb.h"

class PCBList{
	struct Element{
		PCB* val;
		Element *next;
	};
public:
    Element *first, *end;
	PCBList();
	~PCBList();
	void put(PCB* val);
    struct Iterator{
        private:
            PCBList *queue;
            Element *value;
            Element *prev;
        public:
            Iterator(PCBList *);
            void remove();
            PCB* get();
            void next();
            unsigned end();
    };

    Iterator getIterator();
};

#endif
