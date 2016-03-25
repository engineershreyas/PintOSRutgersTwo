#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
bool argCheck(struct intr_frame* f, int numberOfArgs);

#endif /* userprog/syscall.h */
