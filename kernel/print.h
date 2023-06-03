#ifndef KERNEL_PRINT
#define KERNEL_PRINT
#include "types.h"
void console_init();
void print_str(char* str);
void print_char(char c);
void print_dex(uint32_t n);
void print_hex(uint32_t n);


#endif /* KERNEL_PRINT */
