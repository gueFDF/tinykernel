#ifndef USERPROG_SYSCALL_INIT
#define USERPROG_SYSCALL_INIT
#include "types.h"
#define SYS_GETPID 0
#define SYS_WRITE 1
#define SYS_MALLOC 2
#define SYS_FREE 3
#define SYS_OPEN 4
#define SYS_CLOSE 5
#define SYS_READ 6
#define SYS_LSEEK 7
#define SYS_UNLINK 8
#define SYS_MKDIR 9
void syscall_init(void);
uint32_t sys_getpid(void);
#endif /* USERPROG_SYSCALL_INIT */
