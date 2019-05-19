#include <dos.h>
#include "pcb.h"
extern int userMain(int argc, char* argv[]);

void initSystem();
void restoreSystem();

int main(int argc, char* argv[]){

	initSystem();

	int result = userMain(argc, argv);

	restoreSystem();

	return result;
}

void initSystem(){
    PCB::initMain();
    PCB::initIdle();

    PCB::initTimer();
}

void restoreSystem(){
    PCB::restoreTimer();

    //IVTEntry restore

    PCB::killIdle();
    PCB::killMain();
}