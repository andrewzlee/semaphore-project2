/* semaphore.c
contains all relevant prj2 code
*/

/*Global variables*/
typedef int semaphore;
#define NULL 0
#define BINARY 0
#define MULTIVALUED 1
semaphore create_semaphore(int type, int initial_value);
int give(semaphore s); //up
int take(semaphore s); //down
int delete_semaphore(semaphore s);

int binary;
int arrayMulti[100]; // 100 resources avaiable at a time

int array2[100][20]; //create queue


semaphore create_semaphore(int type, int initial_value){
	/* type = binary (0) or multivalued (1) */
	/* */
	int result = 0;
  	semaphore s = initial_value; // n resources are used

  	if (type){ 
  		//multivalued
  		arrayMulti = {0};
  	}
  	else {
  		//binary
  		binary = 0;
  	}


	//do creation stuff
	if (failed)
		return NULL
	else
		return 0 //int value that can be used to find the same semaphore created
}

/*Replace give/up and take/down */
//up
int give(semaphore s){
	//do give

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
}

//down
int take(semaphore s){
	//do take

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
}

int delete_semaphore(semaphore s){
	//destroy semaphore

	if (handleValid){

		//release resources associated with semaphore and allow them to be reused
		return -1;
	}
}