#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/synch.h"

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

	thread_exit ();
}

