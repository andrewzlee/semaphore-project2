README

Andrew Lee 

Contents: patch file, README, test code, man pages

--Patch file--
Patch required for project to work.

place "project2-submission.patch" into /usr/src/ before running "patch -p1 < project2-submission.patch"

afterwards, run "make build && make install && reboot" to create the new kernel so that the test code will run as expected


--Running test code--
"make test" will run david's and my code


--Documentation--

A semaphore is implemented through syscalls and library functions. The main semaphore structure concept was provided by the TAs. The structure contains a value, type, queue and reference list. When a process is begun, an empty pointer array is created for 100 semaphores. A max of 100 semaphores is allowed; a max of 20 processes can be in the queue, and a max of 40 processes can be referenced.

I first mapped system call numbers onto the routines that perform them. I used four empty slots for system calls do_create_semaphore, do_up, do_down, do_delete_semaphore.  These methods were implemented in semaphore.c. Three additional functions also reside in semaphore.c--- add_reference, remove_reference and wake.  All syscalls/functions were also defined in proto.h as prototypes. Lastly, I defined the system call numbers in /include/minix/callnr.h.

The four syscalls follow the project spec and have been tested rigorously to contain no bugs. Four library calls were also created to call the syscalls.  Implementation follows the same guidelines as project 1. New project definitions were declared in usr/src/include/unistd.h.

Forkexit.c was modified to call add_reference and remove_reference in do_fork and do_exit respectively. Add_reference adds the current process to the reference list when fork is called. This provides knowledge that that process is aware of the semaphore. When the process is removed, it's knowledge of the semaphore is also removed from the list. When no PIDs are in the list, the semaphore is automatically marked for deletion.

Wake() is an internal function that wakes up the suspended process through the message passing system.

All tests were passed from the provided code along with my own testing. 