#ifndef LIB_USER_SYSCALL
#define LIB_USER_SYSCALL

#include <types.h>
enum SYSCALL_NR {
  SYS_GETPID,
  SYS_WRITE,
  SYS_MALLOC,
  SYS_FREE,
  SYS_OPEN,
  SYS_CLOSE
};

uint32_t getpid(void);
uint32_t write(int32_t fd, const void* buf, uint32_t count);
void* malloc(uint32_t size);
void free(void* ptr);
int open(const char* pathname, int flags);
int close(int fd);

#endif /* LIB_USER_SYSCALL */
