#ifndef _prepare_h_
#define _prepare_h_

#include "ivtentry.h"
#include <stdio.h>
#include "pcb.h"
#include "schedule.h"

#define PREPAREENTRY(ivtNo, x)\
	void interrupt Ievent##ivtNo(...);\
	\
	static IVTEntry Ientry##ivtNo = IVTEntry(ivtNo, Ievent##ivtNo);\
	\
	void interrupt Ievent##ivtNo(...){\
		Ientry##ivtNo.signal();\
		if(x)\
			Ientry##ivtNo.old();\
		PCB::dispatch();\
    }

#endif
