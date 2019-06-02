#ifndef _ivtentry_h_
#define _ivtentry_h_
#include "event.h"

typedef void interrupt(*pointerInterrupt)(...);

#define IVT_SIZE 256

class KernelEv;

class IVTEntry{
	IVTNo ivtNo;
	pointerInterrupt oldInterrupt;
    static IVTEntry* entries[IVT_SIZE];
    KernelEv* ev;
public:
	IVTEntry(IVTNo ivtNo, pointerInterrupt newInterrupt);
	~IVTEntry();
	void signal();
	void old();
    static void regEvent(KernelEv* ev);
};

#endif
