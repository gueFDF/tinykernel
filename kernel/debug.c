#include "debug.h"

#include "console.h"
#include "interrupt.h"

void panic_spin(char *filename, int line, const char *func,
                const char *condition) {
  intr_disable();  // 关闭中断

  console_write("\n\n\n!!!!! error !!!!!\n");
  console_write("filename:");
  console_write(filename);
  console_write_char('\n');

  console_write("line:");
  console_write_dec(line);
  console_write_char('\n');

  console_write("funtion:");
  console_write((char *)func);
  console_write_char('\n');

  console_write("condition:");
  console_write((char *)condition);
  console_write_char('\n');

  while (1);
}