#ifndef LIB_USER_SYSCALL
#define LIB_USER_SYSCALL

#include <stdint.h>
typedef int16_t pid_t;

enum SYSCALL_NR {
  SYS_GETPID,
  SYS_WRITE,
  SYS_MALLOC,
  SYS_FREE,
  SYS_OPEN,
  SYS_CLOSE,
  SYS_READ,
  SYS_LSEEK,
  SYS_UNLINK,
  SYS_MKDIR,
  SYS_OPENDDIR,
  SYS_CLOSEDIR,
  SYS_READDIR,
  SYS_REWINDDIR,
  SYS_RMDIR,
  SYS_FORK
};

uint32_t getpid(void);
uint32_t write(int32_t fd, const void* buf, uint32_t count);
void* malloc(uint32_t size);
void free(void* ptr);
int open(const char* pathname, int flags);
int close(int fd);
int read(int fd, void* buf, size_t count);
int lseek(int fd, int offset, int whence);
int unlink(const char* pathname);
int32_t mkdir(const char* pathname);
struct dir* opendir(const char* name);
int32_t closedir(struct dir* dir);
struct dir_entry* readdir(struct dir* dir);
void rewinddir(struct dir* dir);
int32_t rmdir(const char* pathname);
pid_t fork();
#endif /* LIB_USER_SYSCALL */
