#include "console.h"
#include "debug.h"
#include "init.h"
#include "memory.h"
int main(void) {
  // console_clear();
  console_write("I am kernel ! \n");

  console_write_hex(9454);
  console_write("\n");
  init_all();

  void* addr = get_kernel_pages(3);
  console_write("\n get_kernel_page start vaddr is ");
  console_write_hex(voidptrTouint32(addr));
  console_write_char('\n');
  while (1)
    ;

  return 0;
}