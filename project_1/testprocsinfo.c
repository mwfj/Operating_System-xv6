/**
 * @author: Wufangjie Ma
 * The goal of the program is simple:
 * to add a system call to xv6.
 * Your system call, getprocsinfo() ,
 * simply returns the PID value and process name for each of the processes that exists in the system at the time of the call.
 **/
#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "procinfo.h"


int stdout = 1;

void forked(){
    if(fork()){

    }else{
        sleep(3);
        exit();
    }
}


void testgetprocsinfo(){
    struct procinfo procs_info[NPROC];
    int i;
    int num_procs = 0;
    num_procs = getprocsinfo(procs_info);
    if (num_procs < 0) {
        printf(stdout,"Get process num failed, num_procs < 0");
        exit();
    }else{
        printf(stdout,"The current process number is %d\n", num_procs);
        num_procs = getprocsinfo(procs_info);
        printf(stdout,"Test1: The current process number is %d\n",num_procs);
        for(i = 0; i < 5; i++)
        {
            forked();
        }
        num_procs = getprocsinfo(procs_info);
        printf(stdout, "Test2: The current process number is %d\n", num_procs);
        for(i = 0; i < 3; i++)
        {
            wait();
        }
        num_procs = getprocsinfo(procs_info);
        printf(stdout, "Test3: The current process number is %d\n", num_procs);
        exit();
    }



}

int main(int argc, char *argv[])
{

    testgetprocsinfo();
    exit();
}
