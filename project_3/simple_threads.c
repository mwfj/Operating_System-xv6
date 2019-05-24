#include "types.h"
#include "stat.h"
#include "user.h"
#include "kthread.h"


volatile int array[30];

void test1(void* arg)
{
    printf(1, "array[0] = %d\n", array[0]);    
    array[10] = 1500;
    exit();
}

int main(int argc, char *argv[])
{

    array[0] = 400; 
    for (int i = 1; i < 30; i++)
    {
        array[i] = i;
    }

    printf(1, "parent: array[0] = %d\n", array[0]);
   
    kthread_t thread = thread_create(test1, 0);
    //printf(1,"---------------------thread created completed\n");
    thread_join(thread);
    printf(1, "array[10] = %d\n", array[10]);
    exit();
}
