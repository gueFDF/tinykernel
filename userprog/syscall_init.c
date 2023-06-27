#include "syscall_init.h"

#include "console.h"
#include "fs.h"
#include "memory.h"
#include "print.h"
#include "string.h"
#include "thread.h"
#include "types.h"
#define syscall_nr 32
typedef void* syscall;
syscall syscall_table[syscall_nr];

// 获取当前任务的pid
uint32_t sys_getpid(void) { return runing_thread()->pid; }

/*初始化系统调用*/
void syscall_init(void) {
  console_write("syscall_init start\n");
  syscall_table[SYS_GETPID] = sys_getpid;
  syscall_table[SYS_WRITE] = sys_write;
  syscall_table[SYS_MALLOC] = sys_malloc;
  syscall_table[SYS_FREE] = sys_free;
  syscall_table[SYS_OPEN] = sys_open;
  syscall_table[SYS_CLOSE] = sys_close;
  syscall_table[SYS_READ] = sys_read;
  syscall_table[SYS_LSEEK]=sys_lseek;
  console_write("syscall_init done\n");
}
