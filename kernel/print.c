#include "print.h"

#include "console.h"
#include "sync.h"

static struct lock console_lock;  // 控制台锁

// 初始化控制台
void console_init() { lock_init(&console_lock); }

// 获取锁
inline static void console_acquire() { lock_acquire(&console_lock); }

// 释放锁
inline static void console_release() { lock_release(&console_lock); }

// 输出字符串
void print_str(char* str) {
  console_acquire();
  console_write(str);
  console_release();
}

// 输出字符
void print_char(char c) {
  console_acquire();
  console_write_char(c);
  console_release();
}

// 以十进制输出
void print_dex(uint32_t n) {
  console_acquire();
  console_write_dec(n);
  console_release();
}

// 以16进制输出
void print_hex(uint32_t n) {
  console_acquire();
  console_write_hex(n);
  console_release();
}
