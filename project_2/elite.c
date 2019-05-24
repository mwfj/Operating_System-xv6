#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int main(int argc, char const *argv[]) {
  int* page2 = (int*)shmem_access(2);
  printf(1, "page2 was %d\n", page2[0]);
  page2[0] = 1337;
  printf(1, "elite set page2 to %d\n", page2[0]);
  exit();
}
