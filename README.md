This Repo is the assignments of CPSC-8240-Advanced Operating System(Based on xv6)

This project is adding some new  functions to the [xv6 system](https://pdos.csail.mit.edu/6.828/2012/xv6.html) developed by [MIT PDOS](https://pdos.csail.mit.edu/)

This project mainly divided into four part:

+ **Project1**: xv6 intro: This first project is just a warmup, and thus relatively light on work. 
The goal of the project is simple: to add a system call to xv6. Your system call, getprocsinfo() , 
simply returns the PID value and process name for each of the processes that exists in the system at the time of the call.

+ **Project2**: Null pointer and share memory
	- **Part A**: Null-pointer Dereference: In xv6, the VM system uses a simple two-level page table as discussed in class. As it currently is structured, user code is loaded into the very ﬁrst part of the address space. Thus, if you dereference a null pointer, you will not see an exception (as you might expect); rather, you will see whatever code is the ﬁrst bit of code in the program that is running.the thing you might do is create a program that dereferences a null pointer.
	- **PartB**:  In this part of the project, you'll explore how to add a shared-memory page to processes that are interested in communicating through memory. The basic process will be simple: there is a new system call you must create, called `void *shmem_access(int page_number)` , which should make a shared page available to the process calling it.<br> One other call is needed: `int shmem_count(int page_number).` This call tells you, for a particular shared page, how many processes currently are sharing that page.
	
+ **Project3**: In this project, you'll be adding real kernel threads to xv6. Specifically, you'll do three things. <br>**First**, you'll define a new system call to create a kernel thread, called `clone()` , as well as one to wait for a thread called` join() `. <br>**Then**, you'll use `clone()` to build a little thread library, with a `thread_create() `call and `lock_acquire() `and `lock_release()` functions.<br> **Finally**, you'll show these things work by using the TA's tests. 

+ **Project4**:
	- **Part A** : xv6 scheduler: to be done in xv6 OS environment.<br> In this part, you'll be putting a new scheduler into xv6. It is called a simple priority-based scheduler .<br> The basic idea is simple: assign each running process a priority, which is an integer number, in this case either 1 (low priority) or 2 (high priority). At any given instance, the scheduler should run processes that have the high priority (2). If there are two or more processes that have the same high priority, the scheduler should round-robin between them. A low-priority (level 1) process does NOT run as long as there are high-priority jobs available to run.
	- **Part B**: File System:
		1. file system checker: to be done on the lab machines, so you can learn more about programming in C on a typical UNIX-based platform (Linux).<br> In this part, you will be developing a working file system checker. A checker reads in a file system image and makes sure that it is consistent. When it isn't, the checker takes steps to fix the problems it sees; however, we won't be doing any fixes this time to keep your life a little simpler.<br>We will use the xv6 file system image as the basic image that we will be reading and checking. The file fs.h includes the basic structures you need to understand, including the superblock, on disk inode format (struct dinode), and directory entry format (struct dirent). The tool mkfs.c will also be useful to look at, in order to see how an empty file-system image is created.
		
		2.  File system integrity: to be done in the xv6 environment.In this part, you'll be changing the existing xv6 file system to add protection from data corruption. In real storage systems, silent corruption of data is a major concern, and thus many techniques are usually put in place to detect (and recover) from blocks that go bad.<br>Specifically, you'll do three things. First, you'll modify the code to allow the user to create a new type of file that keeps a **checksum** for every block it points to. Checksums are used by modern storage systems in order to detect silent corruption.


=========================================================================


**The Information below comes from the Original XV6**


xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern x86-based multiprocessor using ANSI C.

ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)). See also https://pdos.csail.mit.edu/6.828/, which
provides pointers to on-line resources for v6.

xv6 borrows code from the following sources:
    JOS (asm.h, elf.h, mmu.h, bootasm.S, ide.c, console.c, and others)
    Plan 9 (entryother.S, mp.h, mp.c, lapic.c)
    FreeBSD (ioapic.c)
    NetBSD (console.c)

The following people have made contributions: Russ Cox (context switching,
locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
Clements.

We are also grateful for the bug reports and patches contributed by Silas
Boyd-Wickizer, Anton Burtsev, Cody Cutler, Mike CAT, Tej Chajed, eyalz800,
Nelson Elhage, Saar Ettinger, Alice Ferrazzi, Nathaniel Filardo, Peter
Froehlich, Yakir Goaron,Shivam Handa, Bryan Henry, Jim Huang, Alexander
Kapshuk, Anders Kaseorg, kehao95, Wolfgang Keller, Eddie Kohler, Austin
Liew, Imbar Marinescu, Yandong Mao, Matan Shabtay, Hitoshi Mitake, Carmi
Merimovich, Mark Morrissey, mtasm, Joel Nider, Greg Price, Ayan Shafqat,
Eldar Sehayek, Yongming Shen, Cam Tenny, tyfkda, Rafael Ubal, Warren
Toomey, Stephen Tu, Pablo Ventura, Xi Wang, Keiichi Watanabe, Nicolas
Wolovick, wxdao, Grant Wu, Jindong Zhang, Icenowy Zheng, and Zou Chang Wei.

The code in the files that constitute xv6 is
Copyright 2006-2018 Frans Kaashoek, Robert Morris, and Russ Cox.

ERROR REPORTS

Please send errors and suggestions to Frans Kaashoek and Robert Morris
(kaashoek,rtm@mit.edu). The main purpose of xv6 is as a teaching
operating system for MIT's 6.828, so we are more interested in
simplifications and clarifications than new features.

BUILDING AND RUNNING XV6

To build xv6 on an x86 ELF machine (like Linux or FreeBSD), run
"make". On non-x86 or non-ELF machines (like OS X, even on x86), you
will need to install a cross-compiler gcc suite capable of producing
x86 ELF binaries (see https://pdos.csail.mit.edu/6.828/).
Then run "make TOOLPREFIX=i386-jos-elf-". Now install the QEMU PC
simulator and run "make qemu".
