#ifndef KERNEL_SYNC
#define KERNEL_SYNC
#include "list.h"
#include "thread.h"
#include "types.h"

/*信号量结构*/
struct semaphore {
  uint8_t value;
  struct list waiters;
};

/*锁结构*/
struct lock {
  struct task_struct* holder;  // 锁的持有者
  struct semaphore semaphore;  // 用二元信号实现锁
  uint32_t holder_repeat_nr;   // 锁的持有者重复申请锁的使用次数
};

void seam_init(struct semaphore* psema, uint8_t value);
void lock_init(struct lock* plock);
void sema_down(struct semaphore* psema);
void sema_up(struct semaphore* psema);
void lock_acquire(struct lock* plock);
void lock_release(struct lock* plock);

#endif /* KERNEL_SYNC */