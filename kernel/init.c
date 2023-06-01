#include "init.h"

#include "console.h"
#include "interrupt.h"
#include "memory.h"
#include "print.h"
#include "thread.h"
#include "timer.h"
/*负责初始化所有模块 */
void init_all() {
  console_write("init_all\n");
  idt_init();     // 初始化中断
  timer_init();   // 初始化PIT
  mem_init();     // 内存池初始化
  thread_init();  // 初始化线程环境
  console_init();
}