#include "syscall.h"
#include "types.h"
#include "user.h"

#define NULL 0
#define stdout 1
int main()
{
    printf(stdout, "This is a test for NULL pointer deference \n");
    int *p = NULL;
    printf(1, "*p: %d \n",*p);
    exit();
}
