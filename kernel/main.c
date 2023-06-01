#include "console.h"
#include "debug.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
#include "sync.h"
#include "thread.h"

struct lock lc;
void k_thread_a(void*);
void k_thread_b(void*);

int main(void) {
  // console_clear();
  console_write("I am kernel ! \n");

  console_write_hex(9454);
  console_write("\n");
  init_all();
  thread_start("k_thread_a", 31, k_thread_a, "argA ");

  thread_start("k_thread_b", 31, k_thread_b, "argB ");
  lock_init(&lc);
  intr_enable();
  while (1) {
    lock_acquire(&lc);
    console_write("Main ");
    lock_release(&lc);
  }

  return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
  /* 用 void*来通用表示参数,
 被调用的函数知道自己需要什么类型的参数,自己转换再用 */

  char* para = arg;

  while (1) {
    lock_acquire(&lc);
    console_write(para);
    lock_release(&lc);
  }
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {
  /* 用 void*来通用表示参数,
 被调用的函数知道自己需要什么类型的参数,自己转换再用 */

  char* para = arg;

  while (1) {
    lock_acquire(&lc);
    console_write(para);
    lock_release(&lc);
  }
}
