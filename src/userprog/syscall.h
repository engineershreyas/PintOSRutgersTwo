#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

//shreyas methods
bool create(const char *file, unsigned initial_size);
bool remove(const char *file);
int  open(const char *file);
int  filesize(int fd);

#endif /* userprog/syscall.h */
