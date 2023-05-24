#include "init.h"

#include "console.h"
#include "interrupt.h"

/*负责初始化所有模块 */
void init_all() {
  console_write("init_all\n");
  idt_init();  // 初始化中断
}