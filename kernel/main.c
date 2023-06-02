#include "console.h"
#include "debug.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
#include "print.h"
#include "sync.h"
#include "thread.h"

int main(void) {
  console_write("I am kernel ! \n");

  init_all();
  intr_enable();
  while (1) {
  }

  return 0;
}

