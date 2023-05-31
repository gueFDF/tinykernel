#include "timer.h"

#include "common.h"
#include "console.h"
#include "debug.h"
#include "interrupt.h"
#include "thread.h"

uint32_t ticks;  // ticks 是内核自中断开启以来总共的嘀嗒数
// 设置控制字寄存器，并且设置计数初始寄存器
static void frequency_set(uint8_t counter_port, uint8_t counter_on, uint8_t rwl,
                          uint8_t counter_mode, uint16_t counter_value) {
  uint8_t pit = counter_on << 6 | rwl << 4 | counter_mode << 1;
  // 设置控制寄存器
  outb(PIT_CONTROL_PORT, pit);

  // 设置计数初始寄存器
  outb(counter_port, (uint8_t)counter_value);       // 低8位
  outb(counter_port, (uint8_t)counter_value >> 8);  // 高8位
}

/*时钟中断处理函数*/
static void intr_timer_handler(void) {
  struct task_struct* cur_thread = runing_thread();
  ASSERT(cur_thread->stack_magic == STACK_MAGIC);  // 检查PCB栈是否溢出

  cur_thread->elapsed_ticks++;  // 记录次线程占用CPU的时间
  ticks++;
  if (cur_thread->ticks == 0) {  // 若进程时间片用完,就开始调度新的进程上 cpu
    schedule();                  // 进行调度
  } else {
    cur_thread->ticks--;  // 将当前进程的时间片-1
  }
}

// 初始化PIT8253
void timer_init() {
  console_write("time_init start\n");
  /* 设置 8253 的定时周期,也就是发中断的周期 */
  frequency_set(COUNTER0_PORT, COUNTER0_NO, READ_WRITE_LATCH, COUNTER_MODE,
                COUNTER0_VALUE);

  register_handler(0x20, intr_timer_handler);  // 注册中断处理函数
  console_write("timer_init done\n");
}
