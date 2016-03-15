#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "threads/synch.h"
#include "threads/thread.h"

struct lock file_lock;

static void syscall_handler (struct intr_frame *);

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
  thread_exit ();
}

bool create(const char *file, unsigned initial_size){

  bool success = false;
  lock_acquire(&file_lock);
  success = filesys_open(file,initial_size);
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
