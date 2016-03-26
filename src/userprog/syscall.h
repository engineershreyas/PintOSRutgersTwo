#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <stdint.h>

void syscall_init (void);

//shreyas methods
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int  open (const char *file);
int  filesize (int fd);
void close (int fd);

//helper methods
int add_file_to_thread(struct file *f);
struct file* find_file_in_thread(int fd);

#endif /* userprog/syscall.h */
