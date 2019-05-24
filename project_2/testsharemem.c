#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

void failure_msg(void){
    printf(1," TEST FAILED\n");
    exit();
}

int main(){
    
    char *test;
    char value = 'a';
    char *compare = &value;
    int pid = -1;

    if ((pid = fork()) ==0 ) {
        //create first child

        if (fork() ==0 ) {
        // Grand child create
            test = (char*)shmem_access(0);
            int grand_child_count = shmem_count(0);
            printf(1,"Grand child create by using number zero share page and share by %d processes\n", grand_child_count);
            *test = 'b';
            *compare = 'c';
            printf(1,"The grand child address is %x, test value:%c, compare: %x, compare's value: %c\n",test,*test, compare,*compare);
        }

        wait();
        test = (char*)shmem_access(0);
        int first_child_count = shmem_count(0);
        printf(1,"First child create by using number zero share page and share by %d processes\n", first_child_count);
        *test = 'd';
        printf(1,"The first child address is %x, test value:%c, compare: %x, compare's value: %c\n",test,*test, compare,*compare);

    }else{
        //create parent process
        wait();
        test = (char*)shmem_access(0);
        int father_count = shmem_count(0);
        printf(1,"Father process create by using number zero share page and share by %d processes\n", father_count);
        printf(1,"The parent process address is %x, test value:%c, compare: %x, compare's value: %c\n",test,*test, compare,*compare);
    }
    printf(1,"pid:%d\n", pid);
    exit();
}