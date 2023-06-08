#ifndef LIB_USER_SYSCALL
#define LIB_USER_SYSCALL

#include <types.h>
enum SYSCALL_NR { SYS_GETPID };

uint32_t getpid(void);
#endif /* LIB_USER_SYSCALL */
