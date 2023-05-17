#include "print.h"
#include "stdint.h"
void print_str(char* str) {
  int len = 0;
  while (str[len]) {
    put_char(str[len]);
    len++;
  }
}