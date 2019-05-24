#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "pstat.h"

int stdout = 1;




int
ps(void)
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
    return 0;
}

void setpritest(void)
{
    printf(stdout, "start setpri test\n");

    int pid = fork();

    if (pid > 0) {
        wait();
        printf(stdout, "PASS: forked proc exited normally after elevation\n\n");
    } else {
        printf(stdout, "forked proc increasing priority to HIGH_PRI\n");

        if (setpri(2) >= 0)
            printf(stdout, "successfully elevated priority\n");
        else
            printf(stdout, "FAILED: could not elevate priority\n");

        printf(stdout, "forked proc sleeping to yield processor\n");
        exit();
    }
}

void busyloop(int spins)
{
    for (volatile int i = 0; i < spins; i++)
    {
        for (volatile int j = 0; j < 1; j++)
        {
            asm volatile("");
        }
    }
}

void pinfotest(void)
{
    printf(stdout, "start pinfo test\n");

    setpri(2);
    int tpid;
    for (int i = 0; i < 5; i++)
    {
        tpid = fork();
        if (!tpid) 
        {
            int pri; // i % 2 + 1; 
            if (i > 2) pri = 2;
            else pri = 1;
            setpri(pri); 
            busyloop(200000000/pri);
            busyloop(200000000/pri);
            if (pri == 2)
            {
                setpri(1);
                busyloop(200000000/pri);
                ps();
            }
            else
            {
                setpri(1);
                busyloop(200000000/pri);
            }
            exit();
        }
        else printf(1, "Created working process %d\n", tpid);
    }
    setpri(1);
    for (int i = 0; i < 5; i++)
    {
        ps(); 
        wait();
    }
}

    int
main(int argc, char *argv[])
{
    setpritest();
    pinfotest();
    exit();
}


/*
$ testpriority
start setpri test
forked proc increasing priority to HIGH_PRI
successfully elevated priority
forked proc sleeping to yield processor
PASS: forked proc exited normally after elevation

start pinfo test
Created working process 5
Created working process 6
Created working process 7
Created working process 8
Created working process 9
forked proc (PID:3) got proc info:
        pid: 1, hticks: 0, lticks: 2
        pid: 2, hticks: 0, lticks: 2
        pid: 3, hticks: 3, lticks: 2
        pid: 5, hticks: 0, lticks: 1
        pid: 6, hticks: 0, lticks: 1
        pid: 7, hticks: 0, lticks: 0
        pid: 8, hticks: 0, lticks: 0
        pid: 9, hticks: 0, lticks: 0
forked proc (PID:8) got proc info:
        pid: 1, hticks: 0, lticks: 2
forked proc (PID:9) got         pid: 2, hticks: 0, lticks: 2
        pid: 3, hticks: 3, lticks: 5
        pid: 5, hticks: 0, lticks: 327
        pid: proc info:
        pid: 1, hticks: 0, lticks: 2
        pid: 2, hticks: 0, lticks: 2
        pid: 3, hticks: 3, lticks6: 5
        pid: 5, hticks: 0, lticks: 327
        pid: 6, hticks: 0, lticks: 327
        pid: 7, hticks: 0, lticks: 32, hticks: 0, lticks: 327
        pid: 7, hticks: 0, lticks: 324
        pid: 8, hticks: 397, lticks: 196
        pid: 9,4
        pid: 8, hticks: 397, lticks: 197
        pid: 9, hticks: 399, lticks: 195
forked proc hticks: 399, lticks: 195
 (PID:3) got proc info:
        pid: 1, hticks: 0, lticks: 2
        pid: 2, hticks: 0, lticks: 2
        pid: 3, hticks: 3, lticks: 5
        pid: 5, hticks: 0, lticks: 330
        pid: 6, hticks: 0, lticks: 330
        pid: 7, hticks: 0, lticks: 327
        pid: 8, hticks: 397, lticks: 199
forked proc (PID:3) got proc info:
        pid: 1, hticks: 0, lticks: 2
        pid: 2, hticks: 0, lticks: 2
        pid: 3, hticks: 3, lticks: 9
        pid: 5, hticks: 0, lticks: 334
        pid: 6, hticks: 0, lticks: 334
        pid: 7, hticks: 0, lticks: 331
forked proc (PID:3) got proc info:
        pid: 1, hticks: 0, lticks: 2
        pid: 2, hticks: 0, lticks: 2
        pid: 3, hticks: 3, lticks: 11
        pid: 5, hticks: 0, lticks: 1195
        pid: 7, hticks: 0, lticks: 1192
forked proc (PID:3) got proc info:
        pid: 1, hticks: 0, lticks: 2
        pid: 2, hticks: 0, lticks: 2
        pid: 3, hticks: 3, lticks: 13
        pid: 7, hticks: 0, lticks: 1194
*/

