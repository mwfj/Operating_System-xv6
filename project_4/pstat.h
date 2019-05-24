#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

struct pstat {
    int inuse;  // whether this process exists (1 or 0)
    int pid;    // the PID of each process
    int hticks; // the number of ticks accumulated at priority 2
    int lticks; // the number of ticks accumulated at priority 1
};


#endif // _PSTAT_H_
