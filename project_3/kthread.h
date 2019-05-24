#ifndef LIBKTHREAD_H

#include "types.h"

typedef struct lock_t
{  
  uint locked;       // Is the lock held?
} lock_t;

typedef struct kthread_t
{
    int pid;
    void* stack;
} kthread_t;

kthread_t thread_create(void (*start_routine)(void *), void *arg);
int thread_join(kthread_t k);
void init_lock(lock_t *lock);
void lock_acquire(lock_t *lock);
void lock_release(lock_t *lock);
#endif
