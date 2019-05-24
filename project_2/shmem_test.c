#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

// Code Written by Nick Glyder 2018
// Modified by Tyler Allen

int *page0, *page1, *page2, *page3;
char* eliteargv[] = {"elite", 0};

// Grab all the shared pages
void init_shmem() {
  page0 = (int*)shmem_access(0);
  page1 = (int*)shmem_access(1);
  page2 = (int*)shmem_access(2);
  page3 = (int*)shmem_access(3);
}

// Can we read/write to shared pages?
void access_pages_test() {
  printf(1, "START ACCESS TEST\n");

  page0[0] = 10;
  page1[0] = 20;
  page2[0] = 30;
  page3[0] = 40;

  printf(1, "page0 access: int = %d\n", page0[0]);
  printf(1, "page1 access: int = %d\n", page1[0]);
  printf(1, "page2 access: int = %d\n", page2[0]);
  printf(1, "page3 access: int = %d\n", page3[0]);

  if (   page0[0] == 10
      && page1[0] == 20
      && page2[0] == 30
      && page3[0] == 40) {
    printf(1, "ACCESS TEST PASS\n");
  } else {
    printf(1, "ACCESS TEST FAIL\n");
  }
}

// Does fork() share pages and keep counts correct?
void fork_ref_test() {
  printf(1, "START FORK COUNT TEST\n");
  int i = 0;
  int pid;

  // make 5 children
  do {
    pid = fork();
    i++;
  } while(pid > 0 && i < 5);

  // Just kill the child
  if (pid == 0) {
    exit();
  } else {
    int count = shmem_count(0);
    printf(1, "ref count for page0 = %d\n", count);
    if(count == 6) {
      printf(1, "COUNT TEST PASSED\n");
    } else {
      printf(1, "COUNT TEST FAILED\n");      
    }
    while(wait() > 0);
  }
  int count = shmem_count(0);
    printf(1, "ref count for page0 = %d\n", count);
    if(count == 1) {
      printf(1, "COUNT TEST PASSED\n");
    } else {
      printf(1, "COUNT TEST FAILED\n");      
    }
}

// Can forked procs communicate through shared pages?
void fork_access_test() {
  printf(1, "START FORK ACCESS TEST\n");
  int pid = fork();

  if (pid == 0) {
    page1[0] = 5656;
    printf(1, "child setting shared mem to 5656\n");
    exit();
  } else {
    printf(1,"parent waiting for child to touch shared memory\n");
    wait();
    printf(1, "parent found %d in page1\n", page1[0]);
    if (page1[0] == 5656) {
      printf(1, "FORK ACCESS TEST PASSED\n");
    } else {
      printf(1, "FORK ACCESS TEST FAILED\n");
    }
  }
}

// Can I exec another proc and communicate with it?
void exec_test() {
  page2[0] = 0;

  int pid = fork();
  printf(1, "pid: %d\n", pid);
  if (pid == 0) {
    exec("elite", eliteargv);
  } else {
    wait();
  }

  // exec proc should set shared int
  printf(1, "found the message %d from exec() proc\n", page2[0]);
  if (1337 == page2[0]) {
    printf(1, "EXEC TEST PASSED\n");
  } else {
    printf(1, "EXEC TEST FAILED\n");
  }
}

int main(int argc, char const *argv[]) {
  init_shmem();
  access_pages_test();
  fork_ref_test();
  fork_access_test();
  exec_test();
  exit();
}
