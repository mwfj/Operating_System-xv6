#include "kthread.h"
#include "param.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

#define PGSIZE (4096)
kthread_t thread_create(void (*start_routine)(void *), void *arg)
{
    //printf(1, "enter thread_create \n");
    int ret;
    // TODO
    // initialize k_thread
    // call clone
    int stack_addr;
    struct lock_t lock;
    init_lock(&lock);
    //add lock to make malloc becoming atom operation
    void* stack = malloc(PGSIZE);
    lock_acquire(&lock);
    // The space allocated to the stack must be a multiple of PGSIZE
    if ((uint)stack % PGSIZE != 0) {
        free(stack);
        stack = malloc(PGSIZE*2);
        stack_addr = (uint)stack;
        stack += (PGSIZE - (uint)stack % PGSIZE);
    }else{
        stack_addr = (uint)stack;
    }
    lock_release(&lock);
    ret = clone(start_routine, arg, stack);
    struct kthread_t kthread;
    kthread.pid = ret;
    //printf(1,"-------------------- \n");
    kthread.stack = (void *)stack_addr;
    //printf(1,"thread_create completed \n");
    return kthread;
}

int thread_join(kthread_t k)
{
    // TODO
    // join the kthread, release stack resources, return
    // printf(1, "enter thread_join \n");
    lock_t lock;
    int ret;
    // printf(1,"---------------------\n");
    // printf(1,"init lock in thread_join\n");
    init_lock(&lock);
    ret = join(k.pid);
    // printf(1,"call join funtion completed \n");
    lock_acquire(&lock);
    free(k.stack);
    lock_release(&lock);
    // printf(1, "thread_join completed\n");
    // printf(1,"---------------------\n");
    return ret;
}

void init_lock(lock_t *lock)
{
    // intialize lock struct
    lock->locked = 0;
  //  printf(1, "lock initial completed\n");
}

void lock_acquire(lock_t *lock)
{
    // TODO
    // how to acquire lock?
    // see spinlock.c
    // note this is user space, not kernel space
    // so some stuff we can't/won't do
    while (xchg(&lock->locked, 1) != 0)
        ;
    __sync_synchronize();
    //printf(1, "locked\n");
}

void lock_release(lock_t *lock)
{
    // TODO
    // see spinlock.c again
    __sync_synchronize();
    xchg(&lock->locked, 0);
    //printf(1, "locked release\n");
}
