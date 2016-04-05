//create_semaphore library

#include <lib.h>      // provides _syscall and message
#include <unistd.h>   // provides function prototype (see step 1 below)

semaphore create_semaphore(int type, int initial_value) {
	if (type < 0 || initial_value < 0){
		return NULL; // no negative values
	}
	else if (type == BINARY && initial_value > 1){
		return NULL;
	}

    message m;      // Minix message to pass parameters to a system call
    
    m.m1_i1 = type;  // set first integer of message to val
    m.m1_i2 = initial_value;
    semaphore s = _syscall(PM_PROC_NR, CREATE_SEMAPHORE, &m);  // invoke underlying system call
    return s;
    //return handle for semaphore as semaphore (int value)
}
