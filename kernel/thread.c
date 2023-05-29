#include "thread.h"

#include "global.h"
#include "memory.h"
#include "types.h"

#define PG_SIZE 4096

/* 由 kernel_thread 去执行 function(func_arg) */
static void kernel_thread(thread_func* function, void* func_arg) {
  function(func_arg);
}

/* 初始化线程栈 thread_stack,
将待执行的函数和参数放到 thread_stack 中相应的位置 */
void thread_create(struct task_struct* pthread, thread_func function,
                   void* func_arg) {
  pthread->self_kstack -= sizeof(struct intr_stack);
}