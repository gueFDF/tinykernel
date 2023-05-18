#include "print.h"
void main(void) {
  put_str("str print\n");
  put_char('k');
  put_char('e');
  put_char('r');
  put_char('n');
  put_char('e');
  put_char('l');
  put_char('\n');
  put_char('1');
  put_char('2');
  //put_char('\b');
  put_char('\n');

  put_int(65535); //0xffff
  while (1);
}