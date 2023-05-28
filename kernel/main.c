#include "console.h"
#include "debug.h"
#include "init.h"
int main(void) {
  // console_clear();
  console_write("I am kernel ! \n");

  console_write_hex(9454);
  console_write("\n");
  init_all();



  while (1)
    ;

  return 0;
}