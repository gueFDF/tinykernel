#ifndef LIB_USER_SYSCALL
#define LIB_USER_SYSCALL

#include <types.h>
enum SYSCALL_NR {
  SYS_GETPID,
  SYS_WRITE,
  SYS_MALLOC,
  SYS_FREE,
  SYS_OPEN,
  SYS_CLOSE,
  SYS_READ,
  SYS_LSEEK
};

uint32_t getpid(void);
uint32_t write(int32_t fd, const void* buf, uint32_t count);
void* malloc(uint32_t size);
void free(void* ptr);
int open(const char* pathname, int flags);
int close(int fd);
int read(int fd, void* buf, size_t count);
int lseek(int fd, int offset, int whence);
#endif /* LIB_USER_SYSCALL */
