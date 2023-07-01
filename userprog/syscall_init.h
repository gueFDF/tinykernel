#ifndef USERPROG_SYSCALL_INIT
#define USERPROG_SYSCALL_INIT
#include "stdint.h"
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
#define SYS_OPENDDIR 10
#define SYS_CLOSEDIR 11
#define SYS_READDIR 12
#define SYS_REWINDDIR 13
#define SYS_RMDIR 14
#define SYS_FORK 15
void syscall_init(void);
uint32_t sys_getpid(void);
#endif /* USERPROG_SYSCALL_INIT */
