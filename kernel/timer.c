#include "timer.h"

#include "common.h"
#include "console.h"

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

// 初始化PIT8253
void timer_init() {
  console_write("time_init start\n");
  /* 设置 8253 的定时周期,也就是发中断的周期 */
  frequency_set(COUNTER0_PORT, COUNTER0_NO, READ_WRITE_LATCH, COUNTER_MODE,
                COUNTER0_VALUE);
  console_write("timer_init done\n");
}
