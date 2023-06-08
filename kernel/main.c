#include "console.h"
#include "debug.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
#include "print.h"
#include "process.h"
#include "sync.h"
#include "syscall.h"
#include "syscall_init.h"
#include "thread.h"

uint32_t ua_pid;
uint32_t ub_pid;

void k_thread_a(void*);
void k_thread_b(void*);
void u_prog_a(void);
void u_prog_b(void);

int main(void) {
  console_write("I am kernel ! \n");

  init_all();

  process_execute(u_prog_a, "user_prog_a");
  process_execute(u_prog_b, "user_prog_b");

  thread_start("k_thread_a", 31, k_thread_a, NULL);
  thread_start("k_thread_b", 31, k_thread_b, NULL);

  intr_enable();

  print_hex(getpid());
  print_char('\n');

  while (1)
    ;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
  print_hex(sys_getpid());
  print_char('\n');
  print_hex(ua_pid);
  print_char('\n');
  print_hex(ub_pid);
  print_char('\n');
  while (1)
    ;
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {
  print_hex(sys_getpid());
  print_char('\n');
  while (1)
    ;
}

/* 测试用户进程 */
void u_prog_a(void) {
  ua_pid = getpid();
  while (1)
    ;
}

/* 测试用户进程 */
void u_prog_b(void) {
  ub_pid = getpid();
  while (1)
    ;
}