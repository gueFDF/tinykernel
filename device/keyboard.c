#include "keyboard.h"

#include "common.h"
#include "console.h"
#include "global.h"
#include "interrupt.h"

#define KBD_BUF_PORT 0x60  // 键盘 buffer 寄存器端口号为 0x60

/*键盘中断处理程序*/
static void intr_keyboard_handler(void) {
  uint8_t scancode = inb(KBD_BUF_PORT);
  console_write_hex(scancode);
  return;
}

/*键盘初始化*/
void keyboard_init() {
  console_write("keyboard init start\n");
  register_handler(0x21, intr_keyboard_handler);
  console_write("keyboard init done\n");
  return;
}
