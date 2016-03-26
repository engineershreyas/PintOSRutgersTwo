#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <stdint.h>

void syscall_init (void);
bool argCheck(struct intr_frame* f, int numberOfArgs);

//shreyas methods
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int  open (const char *file);
int  filesize (int fd);
void close (int fd);



#endif /* userprog/syscall.h */
