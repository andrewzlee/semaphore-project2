//create_semaphore library

#include <lib.h>      // provides _syscall and message
#include <unistd.h>   // provides function prototype (see step 1 below)

semaphore create_semaphore(int type, int initial_value) {
    message m;      // Minix message to pass parameters to a system call
    
    m.m1_i1 = initial_value;  // set first integer of message to val
    m.m1_i2 = type;

    return _syscall(PM_PROC_NR, CREATE_SEMAPHORE, &m);  // invoke underlying system call
}
