#include "thread.h"

#include "console.h"
#include "debug.h"
#include "global.h"
#include "interrupt.h"
#include "list.h"
#include "memory.h"
#include "string.h"
#include "types.h"
#define PG_SIZE 4096

struct task_struct* main_thread;      // 主线程PCB
struct list thread_ready_list;        // 就绪队列
struct list thread_all_list;          // 所有任务队列
static struct list_elem* thread_tag;  // 用于保存队列中的线程结点

extern void switch_to(struct task_struct* cur, struct task_struct* next);

/*获取当前pcb指针*/
struct task_struct* runing_thread() {
  uint32_t esp;
  asm("mov %%esp,%0" : "=g"(esp));  // 获取当前栈指针

  // 进行对齐，对齐到pcb起始位置(一个pcb只占一个页(0~fff))
  return (struct task_struct*)uint32ToVoidptr(esp & 0xfffff000);
}

/* 由 kernel_thread 去执行 function(func_arg) */
static void kernel_thread(thread_func* function, void* func_arg) {
  intr_enable();  // 打开中断
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
  if (pthread == main_thread) {
    pthread->status = TASK_RUNNING;
  } else {
    pthread->status = TASK_READY;
  }

  pthread->self_kstack = (uint32_t*)((char*)pthread + PG_SIZE);
  pthread->priority = prio;
  pthread->ticks = prio;
  pthread->elapsed_ticks = 0;
  pthread->pgdir = NULL;
  pthread->stack_magic = STACK_MAGIC;  // 魔数
}

struct task_struct* thread_start(char* name, int prio, thread_func fuction,
                                 void* func_arg) {
  // PCB都位于内核空间
  struct task_struct* thread = get_kernel_pages(1);
  init_thread(thread, name, prio);
  thread_create(thread, fuction, func_arg);
  /*确保之前不再队列里面*/
  ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
  /*加入就绪线程队列*/
  list_append(&thread_ready_list, &thread->general_tag);

  /* 确保之前不在队列中 */
  ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
  /*加入全部线程线程队列*/
  list_append(&thread_all_list, &thread->all_list_tag);

  return thread;
}

/*实现任务调度*/
void schedule() {
  // 此时中断应该处于关闭状态
  ASSERT(intr_get_status() == INTR_OFF);
  struct task_struct* cur = runing_thread();
  if (cur->status == TASK_RUNNING) {
    // 该线程时间片已经使用完，将其加入队尾
    ASSERT(!elem_find(&thread_ready_list, &cur->general_tag));
    list_append(&thread_ready_list, &cur->general_tag);
    cur->ticks = cur->priority;
    cur->status = TASK_READY;
  } else {
    /* 若此线程需要某事件发生后才能继续上 cpu 运行,
      不需要将其加入队列,因为当前线程不在就绪队列中 */
  }

  ASSERT(!list_empty(&thread_ready_list));
  thread_tag = NULL;
  thread_tag = list_pop(&thread_ready_list);  // 弹出一个线程上cpu

  struct task_struct* next =
      elem2entry(struct task_struct, general_tag, thread_tag);
  next->status = TASK_RUNNING;
  switch_to(cur, next);
}

/*将kernel中的main函数完善为主线程*/
static void make_main_thread(void) {
  /* 因为 main 线程早已运行,
   * 咱们在 loader.S 中进入内核时的 mov esp,0xc009f000,
   * 就是为其预留 pcb 的,因此 pcb 地址为 0xc009e000,
   * 不需要通过 get_kernel_page 另分配一页*/
  main_thread = runing_thread();
  init_thread(main_thread, "main", 31);

  // main线程正在运行，所以不需要添加在就绪队列当中
  ASSERT(!elem_find(&thread_all_list, &main_thread->all_list_tag));
  list_append(&thread_all_list, &main_thread->all_list_tag);
}

/* 初始化线程环境 */
void thread_init(void) {
  console_write("thread_init start\n");
  list_init(&thread_ready_list);
  list_init(&thread_all_list);

  make_main_thread();
  console_write("thread_init done\n");
}