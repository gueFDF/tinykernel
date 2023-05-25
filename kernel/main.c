#include "console.h"
#include "init.h"
int main(void) {
  // console_clear();
  console_write("I am kernel ! \n");

  console_write_hex(9454);
  console_write("\n");
  init_all();
  
  // 为演示中断处理,在此临时开中断
  asm volatile("sti");

  while (1)
    ;

  return 0;
}