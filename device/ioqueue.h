#ifndef DEVICE_IOQUEUE
#define DEVICE_IOQUEUE
#include "sync.h"
#include "thread.h"
#include "types.h"

#define bufsize 64

/*环形队列*/
struct ioqueue {
  struct lock lock;
  struct task_struct* producter;
  struct task_struct* consumer;

  char buf[bufsize];  // 缓冲区
  int32_t head;       // 队头
  int32_t tail;       // 队尾
};

void ioqueue_init(struct ioqueue* ioq);
bool ioq_full(struct ioqueue* ioq);
char ioq_getchar(struct ioqueue* ioq);
bool ioq_empty(struct ioqueue* ioq);
void ioq_putchar(struct ioqueue* ioq, char byte);
#endif /* DEVICE_IOQUEUE */
