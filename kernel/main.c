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
  uint32_t fd = sys_open("/file1", O_CREAT | O_RDWR);
  char buf[64] = {0};
  int count = 0;
  char* str = "hello G_OS";
  count = sys_write(fd, str, strlen(str));
  sys_close(fd);
  printf("%d closed now\n", fd);

  fd = sys_open("/file1", O_RDWR);
  sys_read(fd, buf, count);
  printf("read : %s\n", buf);
  sys_close(fd);

  unlink("/file1");

  fd = sys_open("/file1", O_RDWR);
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
