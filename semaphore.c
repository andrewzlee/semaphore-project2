#include "pm.h"
#include <minix/callnr.h>
#include <minix/endpoint.h>
#include <limits.h>
#include <minix/com.h>
#include <signal.h>
#include "mproc.h"
#include "param.h"

struct sem_data {
	int value; //num of semaphores
	int type; // binary or multi?
	pid_t queue [20]; // stores processes that are waiting for a semaphore
	pid_t refs [40]; //keeps track of processes that know semaphore id
					
};

/* Global variables */
static struct sem_data * s[100]; //create semaphore array with all info within it
int handle, loc;


int do_create_semaphore(){
	/* type = binary (0) or multivalued (1) */
	
	int typ = m_in.m1_i1; //read in type
	int initial_value = m_in.m1_i2; //read in initial value

	for (int i = 0; i<100; i++){
		if (s[i] == NULL) {		 // if empty populate that address space with semaphore data
			s[i] = calloc(1, sizeof(struct sem_data)); //allocate memory
			s[i]->value = initial_value; 
			s[i]->type = typ;

			for (int j=0; j<40; j++){
				//add the current pid to the refs list
				if (s[i]->refs[j] == NULL){
					s[i]->refs[j] = mp->mp_pid;
					break;
				}
			}
			
			return i+1; //return the handle
		}
	}
	return 0;
}

//up
int do_up(){
	//do give
	handle = m_in.m1_i1;
	loc = handle - 1; //array location

	if (~(handle > 100 || handle < 1 || s[loc] == NULL )){ // check handle valid
		// if binary and already 1 then exit b/c we cannot exceed one
		if (s[loc]->type == 1 && s[loc]->value ==1){
			return 0;
		}
		else if (s[loc]->value > 0){
			s[loc]->value = s[loc]->value + 1;
			return -1;
		}
		//if 0, check queue and let process go if queue is nonempty. otherwise increment
		else if (s[loc]->value == 0){
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
				s[loc]->value = 1; //nothing in queue, increment
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
		//take a resource if available 
		if (s[loc]->value > 0){
			s[loc]->value --;
			return -1; //success
		}
		else if (s[loc]->value == 0){
			//add to queue and then mark off blocked
			for (int i = 0; i < 20; i++){
				if (s[loc]->queue[i] == NULL){
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
		//printf("massive failiure\n");
		return 0; //fail
	}
	else {
		//release resources associated with semaphore and allow them to be reused
		s[loc] = NULL;
		free(s[loc]);
		return -1;
	}
}

void add_reference(pid_t child_pid){
	int exitloop = 0; //toggle to break loop
	for (int i=0; i<100; i++){
		//if semaphore exists 
		if (s[i]!= NULL ){
			//add it to the end of refs
			for (int j=0; j<40; j++){
				//checks for duplicates and the next empty slot
				if (s[i]->refs[j] == NULL || s[i]->refs[j] == child_pid){
					s[i]->refs[j] = child_pid;
					exitloop = 1;
					break;
				}
			}
		}

		if (exitloop){
			break;
		}
	}

}

void remove_reference(){
	//scan through all 100 semaphores
	for (int i=0; i<100; i++){
		//if the slot is NOT empty
		if (s[i]!= NULL ){
			//scan through the refs looking for that specific PID
			for (int j = 0; j<40; j++){
				//if that PID exists 
				if (s[i]->refs[j] == mp->mp_pid ){
					//remove it and shift everything down one
					/* Comment: it's not necessary to shift down one,
					 * but since I already implemented it. Might as well
					 * keep it. 
					 */
					int k = j; //don't mess with the j value
					k++;
					if (s[i]->refs[1] == NULL){ //secial case where there's only 1 element in refs
						s[i]->refs[0] = NULL; 
					}
					else if (k==40){ //special where remove last element from queue
						s[i]->refs[39]=NULL; 
					}
					else{ //every other case
						for (int l = k; l < 40; l++){
							s[i]->refs[l-1] = s[i]->refs[l]; //shift every waiting pid down 1 location
							s[i]->refs[l] = NULL;
						}
					}
					//code for cleanup
					if (s[i]->refs[0]==NULL){
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
	rmp->mp_reply.reply_res = -1; //change suspend to -1

	// mark this process as having a message
	rmp->mp_flags |= REPLY;
}