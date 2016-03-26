#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
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
}

int add_file_to_thread(struct file *f);
struct file* find_file_in_thread(int fd);

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
      struct t_file *tf = list_remove(tf->elem);
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
  struct t_file *tf = malloc(sizeof(struct p_file));
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

  for(el = list_begin(&t->files); el ! list_end(&t->files); el = list_next(elem)){
    struct t_file *tf = list_entry(el, struct t_file, elem);
    if(tf->fd == fd) return tf->file;
  }

  return NULL;

}
