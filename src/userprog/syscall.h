#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/synch.h"
void syscall_init (void);



//--------------------------------------------------------------------------
struct child_prc { // our code for child process; borrowing thread synchronization stuff
	int load; //variable type integer called load
	int pid; //var type integer called processor id, to store pid
	int status; //retains the status, int value, of the pid

	struct lock wait_on_me; //lock for waiting for safety
	struct list_elem elem_of_ch; //list of the elements in the processes

	bool should_wait; //wait 
	bool should_exit; //exit 
};

struct child_prc* add_child_proc (int pid);
void rem_child_proc (struct child_prc *chpr);
struct child_prc* get_child_proc (int pid);
void rem_child_proc (void);

void halt(); //Function 1
void exit(int status); //Function 2 
void exit(int status); //Function 3
int wait(pid_t pid) //Function 4



//--------------------------------------------------------------------------


#endif /* userprog/syscall.h */
