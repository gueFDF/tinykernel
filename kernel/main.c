#include "console.h"
#include "debug.h"
#include "init.h"
#include "memory.h"
#include "thread.h"

void k_thread_a(void*);
int main(void) {
  // console_clear();
  console_write("I am kernel ! \n");

  console_write_hex(9454);
  console_write("\n");
  init_all();
  thread_start("k_thread_a", 31, k_thread_a, "argA ");

  while (1)
    ;

  return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
  /* 用 void*来通用表示参数,
 被调用的函数知道自己需要什么类型的参数,自己转换再用 */

  char* para = arg;

  while (1) {
    console_write(para);
  }
}