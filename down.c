//down library

#include <lib.h>      // provides _syscall and message
#include <unistd.h>   // provides function prototype (see step 1 below)

int down(semaphore s) {
    message m;      // Minix message to pass parameters to a system call
    
    m.m1_i1 = s;  // set first integer of message to val

    return _syscall(PM_PROC_NR, DOWN, &m);  // invoke underlying system call
}
