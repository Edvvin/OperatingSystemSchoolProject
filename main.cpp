#include <dos.h>
#include "pcb.h"
#include "ksemlist.h"
extern int userMain(int argc, char* argv[]);

void initSystem();
void restoreSystem();

int main(int argc, char* argv[]){

	PCB::initSystem();

	int result = userMain(argc, argv);

	PCB::restoreSystem();

	return result;
}