#ifndef LIB_USER_SYSCALL
#define LIB_USER_SYSCALL

#include <types.h>
enum SYSCALL_NR { SYS_GETPID, SYS_WRITE };

uint32_t getpid(void);
uint32_t write(char* str);
#endif /* LIB_USER_SYSCALL */
