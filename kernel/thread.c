#include "thread.h"

#include "global.h"
#include "memory.h"
#include "string.h"
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
  // 先预留中断使用栈的空间
  pthread->self_kstack -= sizeof(struct intr_stack);
  // 再预留线程栈使用的空间
  pthread->self_kstack -= sizeof(struct thread_stack);
  struct thread_stack* kthread_stack =
      (struct thread_stack*)pthread->self_kstack;
  kthread_stack->eip = kernel_thread;
  kthread_stack->func_arg = func_arg;
  kthread_stack->function = function;
  kthread_stack->ebp = 0;
  kthread_stack->ebp = 0;
  kthread_stack->edi = 0;
  kthread_stack->esi = 0;
}

/*初始化线程基本信息*/
void init_thread(struct task_struct* pthread, char* name, int prio) {
  memset(pthread, 0, sizeof(*pthread));
  strcpy(pthread->name, name);
  pthread->status = TASK_RUNNING;
  pthread->priority = prio;

  pthread->self_kstack = (uint32_t*)((char*)pthread + PG_SIZE);
  pthread->stack_magic = STACK_MAGIC;
}

struct task_struct* thread_start(char* name, int prio, thread_func fuction,
                                 void* func_arg) {
  // PCB都位于内核空间
  struct task_struct* thread = get_kernel_pages(1);
  init_thread(thread, name, prio);
  thread_create(thread, fuction, func_arg);

  asm volatile(
      "movl %0, %%esp;\
       pop %%ebp; \
       pop %%ebx;\
       pop %%edi; \
       pop %%esi; ret" ::"g"(thread->self_kstack)
      : "memory");
  return thread;
}