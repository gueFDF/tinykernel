#include "console.h"
#include "debug.h"
#include "dir.h"
#include "fs.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
#include "print.h"
#include "process.h"
#include "shell.h"
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
  put_str("I am kernel\n");
  init_all();
  intr_enable();
  /*************    写入应用程序    *************/
  uint32_t file_size = 17908;
  uint32_t sec_cnt = DIV_ROUND_UP(file_size, 512);

  struct disk* sda = &channels[0].devices[0];
  printk("sec_cnt:%d\n,%s", sec_cnt, sda->name);
  void* prog_buf = sys_malloc(file_size);
  ide_read(sda, 300, prog_buf, sec_cnt);
  int32_t fd = sys_open("/prog_arg", O_CREAT | O_RDWR);
  if (fd != -1) {
    if (sys_write(fd, prog_buf, file_size) == -1) {
      printk("file write error!\n");
      while (1)
        ;
    }
  }
  /*************    写入应用程序结束   *************/
  // cls_screen();
  console_put_str("[rabbit@localhost /]$ ");
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
  uint32_t a = open("/t", O_CREAT);
  close(a);
  while (1)
    ;
}

/* 测试用户进程 */
void u_prog_b(void) {
  while (1)
    ;
}

/*init进程*/
void init(void) {
  uint32_t ret_pid = fork();
  if (ret_pid) {
    while (1)
      ;
  } else {
    my_shell();
  }
  while (1)
    ;
}