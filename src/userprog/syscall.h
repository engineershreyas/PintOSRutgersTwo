#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

//shreyas methods
bool create(const char *file, unsigned initial_size);
bool remove(const char *file);

#endif /* userprog/syscall.h */
