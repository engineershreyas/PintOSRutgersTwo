#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

//shreyas methods
uint32_t *pagedir_create (void);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int  open (const char *file);
int  filesize (int fd);
void close (int fd);

#endif /* userprog/syscall.h */
