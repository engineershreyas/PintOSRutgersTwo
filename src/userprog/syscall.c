#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/init.h"
#include "threads/synch.h" //synchronization tools needed for syscall functions 


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");

  	struct thread* curr = thread_current();
	//point to begining of file stack
   	int *sysPoint = f->esp;

	switch(*sysPoint) {

		case(SYS_HALT) {
			/*
			terminates Pintos by calling the function on the inside;
			should be used seldomly because you'll lose important information
			*/
				shutdown_power_off(); //located in "threads/init.h" file
		}

		case(SYS_EXIT) {
			/*
					terminates the user program and returns STATUS to kernel
					if process's parent waits, status will be returned; 
					return of 0 indicates success; errors indicated by any other value 
				*/
			

					struct thread *current = thread_current(); //holds current thread

					if (are_you_there(current -> pare)) //checks to see if the current thread is a parent
					{
					    /* 
						curent thread points to child process, which points to status of 
						the child process, and sets it equal to the status that the exit
						function takes in as an argument
						*/
					      
					    current -> chpr -> status = status; //status is set
					}

					thread_exit(); //exit the thread to return the exit status

			
		}

		case(SYS_EXEC) {
			/* 
				runs executable taken in from cmd_line, which passes arguments and gives
				that process a program id (pid); return -1, or the pid isn't valid.
				If not valid, it must be that the program cannod load or run. 
				Parent process cannot return from exec until it knows its child process
				was able to load its executable. Synchronization is used. 	

				Function 3 creates child process and should not return until new 
				process is created, or failed be be created.
			*/

				pid_t process_id = process_execute(cmd_line);
				struct child_prc* chpr = get_child_proc(process_id);

			  	while (chpr -> load == 0)
			    {
			    	barrier();
			    }
			  	if (chpr -> load == 2)
			    {
			    	return -1;
			    }

				return process_id; //return the process id number
			
		}

		case(SYS_WAIT) {
			/* 
				Wait until pid terminates; returns status that pid passed to EXIT
				if pid terminated by kernel, return -1
				Note: this function is dependent on the implementation of 
				process_wait(pid)
			*/ 

				//causes process with the specified id to wait
				//Check code modified for this function in the process.c file
				return process_wait(pid);   



		}
	}



  thread_exit ();


}


		
//---------------------------------------------------------------------------------------
struct child_prc* add_child_proc(int pid)
{
  	struct child_prc* chpr = malloc(sizeof(struct child_prc));
  	chpr -> pid = pid;
	chpr -> load = 0;
  	
  	chpr -> should_wait = false; 	
  	chpr -> should_exit = false;

  	lock_init( &chpr -> wait_on_me);
  	list_push_back(&thread_current() -> ch_list,
		 &chpr->elem);
  return chpr;
}

struct child_prc* get_child_proc (int pid)
{
  struct thread *t = thread_current();
  struct list_elem *e;

  for (e = list_begin (&t->ch_list); e != list_end (&t->ch_list);
       e = list_next (e))
        {
          struct child_process *cp = list_entry (e, struct child_process, elem);
          if (pid == chpr->pid)
	    {
	      return chpr;
	    }
        }
  return NULL;
}

void remove_child_proc(struct child_prc *chpr)
{
  list_remove(&chpr -> elem);
  
  free(chpr);
}

void remove_child_proc (void)
{
  struct thread *t = thread_current();
  struct list_elem *next, *e = list_begin(&t->ch_list);

  while (e != list_end (&t->ch_list))
    {
      next = list_next(e);
      struct child_prc *chpr = list_entry (e, struct child_prc,
					     elem);
      list_remove(&chpr -> elem);
      
      free(chpr);
      e = next;
    }
}
//---------------------------------------------------------------------------------------