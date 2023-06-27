#include "console.h"
#include "debug.h"
#include "fs.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
#include "print.h"
#include "process.h"
#include "stdio.h"
#include "stdio_kernel.h"
#include "string.h"
#include "sync.h"
#include "syscall.h"
#include "syscall_init.h"
#include "thread.h"
void k_thread_a(void*);
void k_thread_b(void*);
void u_prog_a(void);
void u_prog_b(void);

int main(void) {
  console_write("I am kernel\n");
  init_all();
  intr_enable();
  process_execute(u_prog_a, "u_prog_a");
  process_execute(u_prog_b, "u_prog_b");
  thread_start("k_thread_a", 31, k_thread_a, "I am thread_a");
  thread_start("k_thread_b", 31, k_thread_b, "I am thread_b");
  uint32_t fd = sys_open("/file1", O_CREAT);
  printf("fd:%d\n", fd);
  sys_close(fd);
  printf("%d closed now\n", fd);

  fd = sys_open("/file1", O_RDWR);
  printf("fd:%d\n", fd);
  char* buf = "12123232";
  char buf2[601] = {0};
  for (int i = 0; i < 600; i++) {
    buf2[i] = 'a';
  }
  buf2[600] = 0;
  sys_write(fd, buf, strlen(buf));
  sys_write(fd, buf2, strlen(buf2));
  sys_close(fd);
  printf("%d closed now\n", fd);
  while (1)
    ;
  return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
  while (1)
    ;
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {
  while (1)
    ;
}

/* 测试用户进程 */
void u_prog_a(void) {
  while (1)
    ;
}

/* 测试用户进程 */
void u_prog_b(void) {
  while (1)
    ;
}
