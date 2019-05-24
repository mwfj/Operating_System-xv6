#include "param.h"
#include "types.h"
#include "user.h"
#include "pstat.h"

int stdout = 1;


int
main(int argc, char *argv[])
{
    // Grab pinfo
    struct pstat* p = (struct pstat*) malloc(sizeof(struct pstat) * NPROC);
    if (!getpinfo(p)) {
      printf(stdout, "forked proc (PID:%d) got proc info:\n", getpid());
    } else {
      printf(stdout, "FAILED: could not get pinfo\n");
    }
    // Print info
    for(int i = 0; i < NPROC; i++) {
      if (p[i].inuse) 
      {
          printf(stdout, "\tpid: %d, hticks: %d, lticks: %d\n",
          p[i].pid, p[i].hticks, p[i].lticks
        );
      }
    }

    exit();
}
