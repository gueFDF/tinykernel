#include "../lib/kernel/console.h"
int main(void) {
  console_clear();
  console_write("hello world1\n");
  console_write("hello world2\n");

  console_write("hello world3\n");

  console_write("hello world4\n");

  while (1)
    ;

  return 0;
}