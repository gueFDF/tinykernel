
#include "console.h"

#include "common.h"
// VGA 的显示缓冲的起点是 0xB8000
static uint16_t *video_memory = (uint16_t *)0xb8000;

// 屏幕"光标"的坐标
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;



// 白字黑底的字符属性
static const uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
// 空格
static const uint16_t blank = 0x20 | (attribute_byte << 8);

static void set_cursor() {
  // 屏幕是 80 字节宽
  uint16_t cursorLocation = cursor_y * 80 + cursor_x;
  // 先设置高8位
  outb(0x3d4, 0x0e);
  outb(0x3d5, cursorLocation >> 8);
  outb(0x3d4, 0x0f);
  outb(0x3d5, cursorLocation & 0xff);
}

void console_clear() {
  int i;
  for (i = 0; i < 80 * 25; i++) {
    video_memory[i] = blank;
  }
  // 将光标重置到开头
  cursor_x = 0;
  cursor_y = 0;
  set_cursor();
}

// 屏幕滚动
static void scroll() {
  // 要超出屏幕范围，需要进行屏幕滚动
  if (cursor_y >= 25) {
    int i;
    // 所有数据往前移动一行
    for (i = 0 * 80; i < 24 * 80; i++) {
      video_memory[i] = video_memory[i + 80];
    }
    // 最后一行使用blanl进行填充
    for (i = 24 * 80; i < 25 * 80; i++) {
      video_memory[i] = blank;
    }
    // 向上移动了一行，所以 cursor_y 现在是 24
    cursor_y = 24;
  }
}

void console_putc_color(char c, real_color_t back, real_color_t fore) {
  uint8_t back_color = (uint8_t)back;
  uint8_t fore_color = (uint8_t)fore;

  uint8_t attribute_byte = (back_color << 4) | (fore_color & 0x0F);
  uint16_t attribute = attribute_byte << 8;

  // 0x08 是退格键的 ASCII 码
  // 0x09 是tab 键的 ASCII 码
  if (c == 0x08 && cursor_x) {
    cursor_x--;
  } else if (c == 0x09) {
    cursor_x =
        (cursor_x + 8) & ~(8 - 1);  // 该操作是将光标移动到下一个8的整数倍处
  } else if (c == '\r') {
    cursor_x = 0;
  } else if (c == '\n') {
    cursor_x = 0;
    cursor_y++;
  } else if (c >= ' ') {
    video_memory[cursor_y * 80 + cursor_x] = c | attribute;
    cursor_x++;
  }

  // 每 80 个字符一行，满80就必须换行了
  if (cursor_x >= 80) {
    cursor_x = 0;
    cursor_y++;
  }

  // 如果需要的话滚动屏幕显示
  scroll();

  // 移动硬件的输入光标
  set_cursor();
}

// 输出字符串(白字黑底)
void console_write(char *cstr) {
  while (*cstr) {
    console_putc_color(*cstr++, rc_black, rc_white);
  }
}

// 输出字符串(自定义颜色)
void console_write_color(char *cstr, real_color_t back, real_color_t fore) {
  while (*cstr) {
    console_putc_color(*cstr++, back, fore);
  }
}

// 屏幕输出一个十六进制的整型数
void console_write_hex(uint32_t n) {
  if (n == 0) {
    console_putc_color('0', rc_black, rc_white);
    return;
  }
  char arr[16];
  int m;
  int i = 0;
  while (n) {
    m = n % 16;
    arr[i++] = m;
    n = n / 16;
  }
  int j;
  for (j = i - 1; j >= 0; j--) {
    if (arr[j] <= 9) {
      console_putc_color(arr[j] + '0', rc_black, rc_white);
    } else {
      console_putc_color('A' + arr[j] - 10, rc_black, rc_white);
    }
  }
}

// 屏幕输出一个十进制的整型数
void console_write_dec(uint32_t n) {
  if (n == 0) {
    console_putc_color('0', rc_black, rc_white);
    return;
  }
  char arr[16];
  int m;
  int i = 0;
  while (n) {
    m = n % 10;
    arr[i++] = m;
    n = n / 10;
  }
  int j;
  for (j = i - 1; j >= 0; j--) {
    console_putc_color(arr[j] + '0', rc_black, rc_white);
  }
}

// 屏幕输出一个char
void console_write_char(char c) {
  console_putc_color(c, rc_black, rc_white);
  return;
}