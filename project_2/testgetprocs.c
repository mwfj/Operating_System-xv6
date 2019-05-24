#include "syscall.h"
#include "types.h"
#include "user.h"

void forked()
{
    if (fork())
    {
    }
    else
    {
        sleep(2);
        exit();
    }
}

void dostuff(void)
{

    int numprocs;
    int i = 0;
    numprocs = getprocs();
    printf (1,"TEST1 %d\n", numprocs);
    for (i = 0; i < 5; i++)
    {
        forked();
    }
    numprocs = getprocs();
    printf (1,"TEST2 %d\n", numprocs);
    for (i = 0; i < 3; i++)
    {
        wait();
    }
    numprocs = getprocs();
    printf (1,"TEST3 %d\n", numprocs);
    for (i = 0; i < 2; i++)
    {
        wait();
    }
    numprocs = getprocs();
    printf (1,"TEST4 %d\n", numprocs);

}

int main()
{
   dostuff(); 
    exit();
}
