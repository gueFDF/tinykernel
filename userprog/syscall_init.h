#ifndef USERPROG_SYSCALL_INIT
#define USERPROG_SYSCALL_INIT
#include "types.h"
#define SYS_GETPID 0
#define SYS_WRITE 1
void syscall_init(void);
uint32_t sys_getpid(void);
uint32_t sys_write(char*);
#endif /* USERPROG_SYSCALL_INIT */
