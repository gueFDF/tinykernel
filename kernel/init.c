#include "init.h"

#include "console.h"
#include "interrupt.h"
#include "timer.h"

/*负责初始化所有模块 */
void init_all() {
  console_write("init_all\n");
  idt_init();  // 初始化中断
  timer_init(); //初始化PIT
}