#ifndef USERPROG_SYSCALL_INIT
#define USERPROG_SYSCALL_INIT
#include "types.h"
#define SYS_GETPID 0
void syscall_init(void);
uint32_t sys_getpid(void);
#endif /* USERPROG_SYSCALL_INIT */
