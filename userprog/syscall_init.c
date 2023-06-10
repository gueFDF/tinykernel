#include "syscall_init.h"

#include "console.h"
#include "print.h"
#include "string.h"
#include "thread.h"
#include "types.h"
#define syscall_nr 32
typedef void* syscall;
syscall syscall_table[syscall_nr];

// 获取当前任务的pid
uint32_t sys_getpid(void) { return runing_thread()->pid; }

uint32_t sys_write(char* str) {
  print_str(str);
  return strlen(str);
}
/*初始化系统调用*/
void syscall_init(void) {
  console_write("syscall_init start\n");
  syscall_table[SYS_GETPID] = sys_getpid;
  syscall_table[SYS_WRITE] = sys_write;
  console_write("syscall_init done\n");
}
