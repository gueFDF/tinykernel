#include "console.h"
#include "debug.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
#include "print.h"
#include "sync.h"
#include "thread.h"

void k_thread_a(void*);
void k_thread_b(void*);

int main(void) {
  console_write("I am kernel ! \n");

  init_all();
  thread_start("k_thread_a", 31, k_thread_a, "argA ");

  thread_start("k_thread_b", 31, k_thread_b, "argB ");
  intr_enable();
  while (1) {
    print_str("Main ");
  }

  return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
  /* 用 void*来通用表示参数,
 被调用的函数知道自己需要什么类型的参数,自己转换再用 */

  char* para = arg;

  while (1) {
    print_str(para);
  }
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {
  /* 用 void*来通用表示参数,
 被调用的函数知道自己需要什么类型的参数,自己转换再用 */

  char* para = arg;

  while (1) {
    print_str(para);
  }
}
