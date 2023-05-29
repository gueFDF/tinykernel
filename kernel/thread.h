#ifndef __THREAD_H_
#define __THREAD_H_

#include "types.h"

typedef void thread_func(void*);

#define STACK_MAGIC 0x19870916  // 自定义魔术
/*进程状态*/
enum task_status {
  TASK_RUNNING,  // 运行
  TASK_READY,    // 准备
  TASK_BLOCKED,  // 阻塞
  TASK_WAITING,  // 等待
  TASK_HANGING,  // 挂起
  TASK_DIED,     // 死亡
};

// 中断产生时一系列压栈操作是压入了此结构中
struct intr_stack {
  uint32_t vec_no;  // 中断号
  /* 下面是八个通用寄存器*/
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp_dummy;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  /* 下面是四个段寄存器*/
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;
  /*以下由 cpu 从低特权级进入高特权级时压入*/
  uint32_t err_code;  // 错误号
  void (*eip)(void);
  uint32_t cs;
  uint32_t eflags;
  void* esp;
  uint32_t ss;
};

// 线程栈thread_stack,内核中位置不确定
struct thread_stack {
  uint32_t ebp;
  uint32_t ebx;
  uint32_t edi;
  uint32_t esi;
  void (*eip)(thread_func* func, void* func_arg);
  void(*unused_retaddr);  // 参数 unused_ret 只为占位置充数为返回地址
  thread_func* function;  // 由 kernel_thread 所调用的函数名
  void* func_arg;         // 由 kernel_thread 所调用的函数所需的参数
};

/***** 以下仅供第一次被调度上 cpu 时使用****/

/* 进程或线程的 pcb,程序控制块 */
struct task_struct {
  uint32_t* self_kstack;  // 各内核线程都用自己的内核栈
  enum task_status status;
  uint8_t priority;  // 线程优先级
  char name[16];
  uint32_t stack_magic;  // 栈的边界标记,用于检测栈的溢出
};

struct task_struct* thread_start(char* name, int prio, thread_func fuction,
                                 void* func_arg);
#endif