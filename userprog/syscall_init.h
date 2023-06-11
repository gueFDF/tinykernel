#ifndef USERPROG_SYSCALL_INIT
#define USERPROG_SYSCALL_INIT
#include "types.h"
#define SYS_GETPID 0
#define SYS_WRITE 1
#define SYS_MALLOC 2
#define SYS_FREE 3
void syscall_init(void);
uint32_t sys_getpid(void);
uint32_t sys_write(char*);
#endif /* USERPROG_SYSCALL_INIT */
