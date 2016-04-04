#include "pm.h"
#include <minix/callnr.h>
#include <minix/endpoint.h>
#include <limits.h>
#include <minix/com.h>
#include <signal.h>
#include "mproc.h"
#include "param.h"

/*semaphore.c*/

/*
int do_setsv() {
  register struct mproc *rmp = mp;

  rmp->shared_val = m_in.m1_i1;

  return OK;
}

int do_getsv() {
  register struct mproc *rmp = find_proc(m_in.m1_i1);

  if(!rmp) {
    return -1;
  }

  mp->mp_reply.m1_i1 = rmp->shared_val;

  return OK;
}
*/

struct sem_data {
	int value;
	int type;
	pid_t queue [20];
	pid_t refs [40]; //keeps track of processes that know semaphore id
					//when do_fork occurs you add to ref table, when do_exit called you remove from ref
};


typedef int semaphore;
#define NULL 0
#define BINARY 0
#define MULTIVALUED 1

/* Global variables */
static struct sem_data * s[100]; //create semaphore array with all info within it
int handleValid  = 0; // temp to compile code


int do_create_semaphore(){
	/* type = binary (0) or multivalued (1) */
	/* */

	int initial_value = m_in.m1_i1;
	int typ = m_in.m1_i2;

	for (int i = 0; i<100; i++){
		if (s[i] == 0) {		 // if empty populate that address space with semaphore data
			s[i] = calloc(1, sizeof(struct sem_data));
			s[i]->value = initial_value;
			s[i]->type = typ;
			break;
		}
	}

	return 0;
	//do creation stuff
	/*if (failed)
		return NULL
	else
		return 0 //int value that can be used to find the same semaphore created
	*/
}

/*Replace give/up and take/down */
//up
int do_up(){
	//do give
/*
	if (handleValid){
		if (s > 0){
			s++;
			return -1;
		}
		else if (s == 0){
			//isEmpty = determine whether queue associated w/ that semaphore is nonempty
			if ( !isEmpty ){ // if non-empty
				//remove first process in queue and mark it as unblocked
			}
			else {
				s++;
				return -1;
			}
		}
	}
	s++;
	*/
}

//down
int do_down(){
	//do take
/*
	if (handleValid){

		while (s <= 0){
			//busy wait
		}

		s--;

		return -1; //success
	}
	else {
		return 0; // invalid handle
	}
	*/
}

int do_delete_semaphore(){
	//destroy semaphore

	if (handleValid){

		//release resources associated with semaphore and allow them to be reused
		return -1;
	}
}