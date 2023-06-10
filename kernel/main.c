#include "console.h"
#include "debug.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
#include "print.h"
#include "process.h"
#include "stdio.h"
#include "sync.h"
#include "syscall.h"
#include "syscall_init.h"
#include "thread.h"

void k_thread_a(void*);
void k_thread_b(void*);
void u_prog_a(void);
void u_prog_b(void);
int prog_a_pid = 0, prog_b_pid = 0;

int main(void) {
  console_write("I am kernel\n");
  init_all();
  thread_start("k_thread_a", 31, k_thread_a, "argA ");
  thread_start("k_thread_b", 31, k_thread_b, "argB ");
  process_execute(u_prog_a, "user_prog_a");
  process_execute(u_prog_b, "user_prog_b");

  intr_enable();
  print_str(" main_pid:0x");
  print_hex(sys_getpid());
  print_char('\n');

  while (1)
    ;
  return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
  // char* para = arg;
  print_str(" thread_a_pid:0x");
  print_hex(sys_getpid());
  print_char('\n');
  while (1)
    ;
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {
  // char* para = arg;
  print_str(" thread_b_pid:0x");
  print_dex(sys_getpid());
  print_char('\n');
  while (1)
    ;
}

/* 测试用户进程 */
void u_prog_a(void) {
  prog_a_pid = getpid();
  printf("u_prog_a pid : %x\n", getpid());
  while (1)
    ;
}

/* 测试用户进程 */
void u_prog_b(void) {
  printf("u_prog_b pid : %x\n", getpid());
  while (1)
    ;
}
