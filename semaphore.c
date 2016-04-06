#include "pm.h"
#include <minix/callnr.h>
#include <minix/endpoint.h>
#include <limits.h>
#include <minix/com.h>
#include <signal.h>
#include "mproc.h"
#include "param.h"

/*TODOS:
figure what the refs [40] array does
--> implement refs for cleanup
*/

struct sem_data {
	int value;
	int type;
	pid_t queue [20];
	pid_t refs [40]; //keeps track of processes that know semaphore id
					//when do_fork occurs you add to ref table, when do_exit called you remove from ref
};

/* Global variables */
static struct sem_data * s[100]; //create semaphore array with all info within it
int handle, loc;


int do_create_semaphore(){
	/* type = binary (0) or multivalued (1) */
	/* */
	
	int typ = m_in.m1_i1;
	int initial_value = m_in.m1_i2;
	//printf("line 57\n");
	for (int i = 0; i<100; i++){
		if (s[i] == NULL) {		 // if empty populate that address space with semaphore data
			//printf("line 60\n");
			s[i] = calloc(1, sizeof(struct sem_data));
			s[i]->value = initial_value;
			s[i]->type = typ;
			//s[i]->queue = {NULL};
			for (int j=0; j<40; j++){
				//add the current pid to the refs list
				if (s[i]->refs[j] == 0){
					s[i]->refs[j] = mp->mp_pid;
					break;
				}
			}
			/* //debug code
			printf("type: %d... val: %d i: %d\n", s[i]->type, s[i]->value, i);
			if (s[0] == NULL){
				printf("current is null\n");
			}
			else {
				printf("current has been changed\n");
			}

			if (s[20] == NULL){
				printf("next is null: \n");
			}
			*/
			//printf("i+1 is: %d\n", i+1);
			return i+1; //return the handle
		}
	}
	//if all 100 are populated then return failure
	printf("all 100 filled\n");
	return 0;
}

//up
int do_up(){
	//do give
	handle = m_in.m1_i1;
	loc = handle - 1; //array location

	if (~(handle > 100 || handle < 1 || s[loc] == NULL )){ // check handle valid
		if (s[loc]->type == 1 && s[loc]->value ==1){
			return 0;
		}
		else if (s[loc]->value > 0){
			s[loc]->value = s[loc]->value + 1;
			return -1;
		}
		else if (s[loc]->value == 0){
			//isEmpty = determine whether queue associated w/ that semaphore is nonempty
			if ( s[loc]->queue[0] != NULL ){ // if non-empty
				//remove first process in queue and mark it as unblocked (wake up the next process)
				wake(s[loc]->queue[0]); //wake up 

				int i = 1;
				while (s[loc]->queue[i] != NULL && (i < 20) ){
					s[loc]->queue[i-1] = s[loc]->queue[i]; //shift every waiting pid down 1 location
					s[loc]->queue[i] = NULL;
					i++;
				}
				return -1;	
			}
			else {
				s[loc]->value = 1; 
				return -1;
			}
		}
	}

	return 0; //failure
	
}

//down
int do_down(){
	//do take
	handle = m_in.m1_i1;
	loc = handle - 1;

	if (~(handle > 100 || handle < 1 || s[loc] == NULL )){ 

		/*
		while (s[loc]->value <= 0){
			//busy wait
		}*/
		if (s[loc]->value > 0){
			s[loc]->value --;
			return -1; //success
		}
		else if (s[loc]->value == 0){
			//add to queue and then mark off blocked
			for (int i = 0; i < 20; i++){
				if (s[loc]->queue[i] == NULL){
					//pid_t pd = getpid();
					printf("%d added by down\n", mp->mp_pid);
					s[loc]->queue[i] = mp->mp_pid;
					break;
				}
			}
			return SUSPEND; //mark off as blocked
		}	
	}
	return 0; // invalid handle
	
}
//destroy semaphore
int do_delete_semaphore(){
	
	handle = m_in.m1_i1;
	loc = handle - 1; //array location

	/*if the handle is out of bounds of the array OR 
	there is no semaphore in that location return FAILURE*/
	if (handle > 100 || handle < 1 || s[loc] == NULL ){
		printf("massive failiure\n");
		return 0; //fail
	}
	else {
		//release resources associated with semaphore and allow them to be reused
		printf("semaphore released and reset to null\n");
		//TODO: perform checks for the queue?
		s[loc] = NULL;
		free(s[loc]);
		return -1;
	}
}

void add_reference(){
	//printf("add. 153\n");

	for (int i=0; i<100; i++){
		//if semaphore exists 
		if (s[i]!= NULL ){
			//add it to the end of refs
			for (int j=0; j<40; j++){
				if (s[i]->refs[j] == NULL){
					s[i]->refs[j] = mp->mp_pid;
					break;
				}
			}
		}

		if (s[i] == NULL){
			break;
		}
	}

}

void remove_reference(){
	//printf("remove. 157\n");
	//scan through all 100 semaphores
	for (int i=0; i<100; i++){
		//if the slot is NOT empty
		if (s[i]!= NULL ){
			//scan through the refs looking for that specific PID
			for (int j = 0; j<40; j++){
				/*
				if (s[i]->refs[j] == NULL){
					if (s[i]->refs[1]==NULL){
						free(s[i])
					}
					break;
				} 
				*/
				//if that PID exists 
				if (s[i]->refs[j] == mp->mp_pid ){
					//remove it and shift everything down one
					int k = j;
					if (k == 0){
						k++;
					}
					while (s[i]->refs[k] != NULL && (k < 40) ){
						s[i]->refs[k-1] = s[i]->refs[k]; //shift every waiting pid down 1 location
						s[i]->refs[k] = NULL;
						k++;
					}

					if (s[i]->refs[1]==NULL){
						//printf("%d freed line 207\n", i);
						s[i] = NULL;
						free(s[i]);
					}
					break;
				}
			}
		}
	}
}

void wake(pid_t p)
{
	register struct mproc *rmp = find_proc(p);

	if (rmp == NULL){
		printf("PID doesn't exist. MASSIVE FAILURE\n");
	}

	// indicate success
	rmp->mp_reply.reply_res = OK;

	// and mark this process as having a message
	rmp->mp_flags |= REPLY;
}