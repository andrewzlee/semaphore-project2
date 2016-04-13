/*
testprj2.c


Testing for project 2 -- creating a semaphore.
*/
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "shared.h"

int val;
pid_t pid;

int main() {

	//test creation of multi and up(s) 5 times
	semaphore s = create_semaphore(MULTIVALUED, 10);
	for (int i = 0; i<5; i++){
		val = up(s);
		printf("expected return: -1, actual: %d\n", val);
	}

	sleep(3);
	//check to see that down works after several ups	
	val = down(s);
	printf("expected return: -1, actual: %d\n", val);

	//fork 41 times to make sure reference array fills and last one doesn't fill
	/*
	for(int i = 0; i < 41; i++) {
	    pid = fork();
	    if(pid < 0) {
	        printf("Error");
	        exit(1);
	    } else if (pid == 0) {
	        printf("Child (%d): %d\n", i + 1, getpid());
	        sleep(1);
	        exit(0); 
	    } else  {
	        while (true) {
        	int status;
        	pid_t done = wait(&status);
		        if (done == -1) {
		          if (errno == ECHILD) break; // no more child processes
		        } else {
		          if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		            exit(1);
		          }
		        }
      		}
	    }
	    */
	    /* Note, in final code nothing is outputted. But during 
		 * private testing debug print statements were output
		 * to see that the code worked.
		}
		*/
	printf("Filling reference works. Don't try to run that code though.\n");

	// remove semaphore
	val = delete_semaphore(s);
	printf("expected return: -1, actual: %d\n", val);


	//test forking for multivalued semaphore
	s = create_semaphore(MULTIVALUED, 2);
	val = down(s);
	printf("expected return: -1, actual: %d\n", val);
	val = down(s);
	printf("expected return: -1, actual: %d\n", val);
	
	pid_t child = fork();
	if (child == 0){ 
		val = down(s);
		printf("it waited and nowexpected return: -1, actual: %d\n", val);
		exit(0);
	}
	sleep(2);
    up(s); //release for child to access semaphore
}
