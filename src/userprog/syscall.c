#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
<<<<<<< HEAD
#include "userprog/process.h"
#include "threads/init.h"
#include "threads/synch.h" //synchronization tools needed for syscall functions 
=======
#include "threads/synch.h"
#include "filesys/filesys.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "filesys/file.h"

struct lock file_lock;

struct t_file {
  struct file *file;
  int fd;
  struct list_elem elem;
};

//helper methods
int add_file_to_thread(struct file *f);
struct file* find_file_in_thread(int fd);
>>>>>>> master


static void syscall_handler (struct intr_frame *);

/*this function is used to check that the arguements for each case 
can be stored withing the file without overflow */
bool argCheck(struct intr_frame* f, int numberOfArgs) {
	//initialized i at 1 so we can use i to mutliply to 4 for an allinged word on the stack past the base address
	for(int i = 1; i <= numberOfArgs; ++i) {
		if(!is_user_vaddr (f->esp+4*(i)) || lookup_page(thread_current()->pagedir, f->esp+4*(i), false) == NULL) {
			return false;
		}
	}
	return true;
}

/* these are the globals that shreyas used, use them
struct lock file_lock;

struct t_file {
	struct file *file;
	int fd;
	struct list_elem elem;
}
*/

void
syscall_init (void)
{
  lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  printf ("system call!\n");

  	struct thread* curr = thread_current();
	//point to begining of file stack
   	int *sysPoint = f->esp;

   	int arg[MAX_ARGS];
  	check_valid_ptr((const void*)f->esp);

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

  case SYS_CREATE:
      {
	get_arg(f, &arg[0], 2);
	arg[0] = user_to_kernel_ptr((const void *) arg[0]);
	f->eax = create((const char *)arg[0], (unsigned) arg[1]);
	break;
      }
    case SYS_REMOVE:
      {
	get_arg(f, &arg[0], 1);
	arg[0] = user_to_kernel_ptr((const void *) arg[0]);
	f->eax = remove((const char *) arg[0]);
	break;
      }
    case SYS_OPEN:
      {
	get_arg(f, &arg[0], 1);
	arg[0] = user_to_kernel_ptr((const void *) arg[0]);
	f->eax = open((const char *) arg[0]);
	break;
      }
    case SYS_FILESIZE:
      {
	get_arg(f, &arg[0], 1);
	f->eax = filesize(arg[0]);
	break;
      }

      case(SYS_READ) {
			if (!argCheck(f,3)) {
				quit(f);
				break;
			}
			int fd = * (int *) (f->esp + 4);
			char *buffer = * (char **) (f->esp + 8);
			int size = * (int *) (f->esp + 12);
			if(buffer == NULL || !is_user_vaddr (buffer) || lookup_page(cur->pagedir, buffer, false) == NULL) {
				quit(f);
				break;
			}
			struct t_file *currFile ;
			currFile = find_file_in_thread(fd);
			if(currFile != NULL) {
				//returns the bytes actually read, as opposed to the bytes that wanted to be read
				f->eax = file_read(currFile->file, buffer, size);
			}
			else if(fd == 0) { //if fd == 0, then we read from keyboard input
				//read from keyboard instead
				input_getc();
				f->eax = size;
			}
			break;
		}

    /*This systemcall writes to a file. the data to be written starts from BUFFER and will be 
		written until BUFFER has written SIZE bytes or the file has ran out of space to be written too */
		case(SYS_WRITE) {
			//check to make sure all arguements fit on the user stack
			argCheck(f, 3);
		  	//acqire lock
		  	lock_acquire(&file_lock);
		  	//start from the pointer BUFFER, which points to the buffer cache
		  	//the thread will refer to it's file table to acquire the file based on its fd.
		  	int fd = *(int*)(f->esp + 4);
		  	char* buffer = *(char**)(f->esp + 8);
		  	unsigned size = *(unsigned*)(f->esp + 12);
		  	//retreave the file from the thread file table
		  	struct t_file currFile;
		  	currFile.file = find_file_in_thread(fd);
		  	//check if the file was found AND the buffer pointer points to cache AND the file pointer points to a user address AND it is on the virtual memory
		  	if (buffer == NULL || !is_user_vadder(buffer) || lookup_page(curr->pagedir, buffer, false) == NULL) {
		  		quit(f);
		  		break;
		  	}
		  	//test if the file has been found successfully
		  	if (currFile.file != NULL) {
		  		currFile.fd = fd;
		  		//write to the file. the function returns the number of bytes written (as opposed to the number of bytes that SHOULD have been written)
		  		f->eax = file_write(currFile->file, buffer, size);
		  	} else if (currFile.fd == 1) { //then we are writing to STDCOUT, as the fd of STDCOUT is 1
		  		//this tests to make sure that the size of SIZE is not too large for the consol to handle. 512 is an arbitrary size based off the pintos docs
		  		if (sizeof(size) < 512)){
					quit(f);
					break;
				}
		  		putbuf(buffer,size);
		  		f->eax = size;
	  	}

	  	/* this system call moves to POS in the file with FD. POS is a byte which will be read or written to */
	  	case(SYS_SEEK) {
	  		argCheck(f, 1);
	  		lock_acquire(&file_lock);
	  		int fd = * (int *) (f->esp + 4);
	  		//assign the desired position of the byte here on the file stack
			unsigned pos = * (unsigned *) (f->esp + 8);
			struct t_file *currFile;
			currFile.file = find_file_in_thread(fd);
			if(currFile.file != NULL) {
				file_seek(currFile.file, pos);
			}
			lock_release(&file_lock);
			break;
	  	}

	  	/* this system call returns the position of the next byte to be read or written to */
	  	case(SYS_TELL) {
	  		argCheck(f, 1);
	  		int fd_no = * (int *) (f->esp + 4);
			struct t_file *currFile;
			currFile.file = find_file_in_thread(fd);
			if(currFile.file != NULL) {
				f->eax = file_tell(currFile.file);
			}
			break;
	  	}


	  	//release lock
		lock_release(&file_lock);
		break;
	  }


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
	
	
  //printf ("system call!\n");
  //thread_exit ();
}

bool create(const char *file, unsigned initial_size){

  bool success = false;
  lock_acquire(&file_lock);
  success = filesys_create(file,initial_size);
  lock_release(&file_lock);

  return success;

}

bool remove(const char *file){

  bool success = false;
  lock_acquire(&file_lock);
  success = filesys_remove(file);
  lock_release(&file_lock);

  return success;

}

int open(const char *file){

  lock_acquire(&file_lock);
  struct file *f = filesys_open(file);
  if(f == NULL){
    lock_release(&file_lock);
    return -1;
  }
  else{
    int fd = add_file_to_thread(f);
    lock_release(&file_lock);
    return fd;
  }

}

void close(int fd){
  lock_acquire(&file_lock);
  struct thread *t = thread_current();
  struct list_elem *el;

  for(el = list_begin(&t->files); el != list_end(&t->files); el = list_next(el)){
    struct t_file *tf = list_entry(el, struct t_file, elem);
    if(tf->fd == fd){
      list_remove(&tf->elem);
      file_close(tf->file);
      free(tf);
      lock_release(&file_lock);
      return;
    }
  }

  lock_release(&file_lock);


}

int filesize(int fd){
  lock_acquire(&file_lock);
  struct file *f = find_file_in_thread(fd);
  if(f == NULL){
      lock_release(&file_lock);
      return -1;
  }
  int len = file_length(f);
  lock_release(&file_lock);
  return len;
}

int add_file_to_thread(struct file *f){
  struct t_file *tf = malloc(sizeof(struct t_file));
  tf->file = f;
  struct thread *t = thread_current();
  tf->fd = t->fd;
  t->fd++;
  list_push_back(&t->files,&tf->elem);

  return tf->fd;
}

struct file* find_file_in_thread(int fd){
  struct thread *t = thread_current();
  struct list_elem *el;

  for(el = list_begin(&t->files); el != list_end(&t->files); el = list_next(el)){
    struct t_file *tf = list_entry(el, struct t_file, elem);
    if(tf->fd == fd) return tf->file;
  }

  return NULL;


}

