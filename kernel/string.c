#include "string.h"
void *memcpy(void *dest, const void *src, uint32_t len) {
  uint8_t *To = (uint8_t *)dest;
  uint8_t *From = (uint8_t *)src;
  while (len--) {
    *To++ = *From++;
  }
  return dest;
}

size_t strlen(const char *s) {
  int ret = 0;
  while (*s) {
    ret++;
    s++;
  }
  return ret;
}
void memset(void *dest, uint8_t val, uint32_t len) {
  uint8_t *dst = (uint8_t *)dest;
  for (int i = 0; i < len; i++) {
    *dst++ = val;
  }
}

void bzero(void *dest, uint32_t len) { memset(dest, 0, len); }

int strcmp(const char *str1, const char *str2) {
  while ((*str1 == *str2) && (*str1 != '\0')) {
    str1++;
    str2++;
  }
  return *str1 - *str2;
}

char *strcpy(char *dest, const char *src) {
  char *temp = dest;
  while (*src) {
    *dest++ = *src++;
  }
  return temp;
}
