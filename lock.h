#ifndef _lock_h_
#define _lock_h_

#define lock PCB::lockFlag++;

#define unlock\
    if(--PCB::lockFlag == 0){\
        if(PCB::contextSwitchFlag){\
            PCB::contextSwitchFlag = 0;\
            dispatch();\
        }\
    }

#define unlockDispatch\
    if(--PCB::lockFlag == 0){\
        PCB::contextSwitchFlag = 0;\
        dispatch();\
    }


#endif