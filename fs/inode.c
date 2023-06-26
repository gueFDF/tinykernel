#include "inode.h"

#include "debug.h"
#include "ide.h"
#include "interrupt.h"
#include "list.h"
#include "memory.h"
#include "string.h"
#include "super_block.h"
#include "thread.h"
#include "stdio_kernel.h"
/*用来存储inode位置*/
struct inode_position {
  bool two_sec;       // inode是否跨扇区
  uint32_t sec_lba;   // inode所在的扇区号
  uint32_t off_size;  // inode在扇区内的字节偏移量
};

/*获取inode所在扇区和扇区内的偏移量*/
static void inode_locate(struct partition* part, uint32_t inode_no,
                         struct inode_position* inode_pos) {
  /*inode_table在硬盘上是连续的(只支持4096个文件)*/
  
  ASSERT(inode_no < 4096);
  uint32_t inode_table_lba = part->sb->inode_table_lba;
  uint32_t inode_size = sizeof(struct inode);
  uint32_t off_size = inode_no * inode_size;  // 字节偏移量

  uint32_t off_sec = off_size / 512;          // 扇区偏移量
  uint32_t off_size_in_sec = off_size % 512;  // 在待查找扇区的起始地址

  // 判断是否跨两个扇区
  uint32_t left_in_sec = 512 - off_size_in_sec;  // 该扇区剩余空间
  if (left_in_sec < inode_size) {                // 判断是否放的下
    // 无法容纳一个inode
    inode_pos->two_sec = true;
  } else {
    inode_pos->two_sec = false;
  }
  inode_pos->sec_lba = inode_table_lba + off_sec;
  inode_pos->off_size = off_size_in_sec;
}

/*将inode写入分区part*/
void inode_sync(struct partition* part, struct inode* inode, void* io_buf) {
  // io_buf 是用于硬盘 io 的缓冲区
  uint8_t inode_no = inode->i_no;
  struct inode_position inode_pos;
  inode_locate(part, inode_no, &inode_pos);  // 获取位置信息
  ASSERT(inode_pos.sec_lba <= (part->start_lba + part->sec_cnt));

  // inode中有三项不需要写入磁盘
  struct inode pure_inode;
  memcpy(&pure_inode, inode, sizeof(struct inode));
  pure_inode.i_open_cnts = 0;
  pure_inode.write_deny = false;
  pure_inode.inode_tag.prev = pure_inode.inode_tag.next = NULL;

  char* inode_buf = (char*)io_buf;
  if (inode_pos.two_sec) {
    /* 跨扇区了读出两个数据*/
    ide_read(part->my_disk, inode_pos.sec_lba, inode_buf, 2);
    // 填入数据
    memcpy((inode_buf + inode_pos.off_size), &pure_inode, sizeof(struct inode));
    // 写回
    ide_write(part->my_disk, inode_pos.sec_lba, inode_buf, 2);
  } else {
    ide_read(part->my_disk, inode_pos.sec_lba, inode_buf, 1);
    memcpy((inode_buf + inode_pos.off_size), &pure_inode, sizeof(struct inode));
    ide_write(part->my_disk, inode_pos.sec_lba, inode_buf, 1);
  }
}

/*根据i节点号返回相应的我i节点*/
struct inode* inode_open(struct partition* part, uint32_t inode_no) {
  // 先在已经打开的inode链表中找inode.此链表相当于缓冲哦
  struct list_elem* elem = part->open_inodes.head.next;
  struct inode* inode_found;
  while (elem != &part->open_inodes.tail) {
    inode_found = elem2entry(struct inode, inode_tag, elem);
    if (inode_found->i_no == inode_no) {
      inode_found->i_open_cnts++;
      return inode_found;
    }
    elem = elem->next;
  }

  /*从缓冲中没有找到*/
  struct inode_position inode_pos;
  inode_locate(part, inode_no, &inode_pos);  // 获取在磁盘中的位置
  struct task_struct* cur = runing_thread();

  /* 为使通过 sys_malloc 创建的新 inode 被所有任务共享, 需要将 inode
 置于内核空间,故需要临时 将 cur_pbc->pgdir 置为 NULL */
  uint32_t* cur_pagedir_bak = cur->pgdir;
  cur->pgdir = NULL;  // 暂时置为空
  inode_found = (struct inode*)sys_malloc(sizeof(struct inode));
  /*恢复pgdir*/
  cur->pgdir = cur_pagedir_bak;
  char* inode_buf;
  if (inode_pos.two_sec) {  // 跨扇区
    inode_buf = (char*)sys_malloc(1024);
    ide_read(part->my_disk, inode_pos.sec_lba, inode_buf, 2);
  } else {
    inode_buf = (char*)sys_malloc(512);
    ide_read(part->my_disk, inode_pos.sec_lba, inode_buf, 1);
  }
  memcpy(inode_found, inode_buf + inode_pos.off_size, sizeof(struct inode));

  // 插入队头，因为最可能被访问到
  list_push(&part->open_inodes, &inode_found->inode_tag);
  inode_found->i_open_cnts = 1;
  sys_free(inode_buf);
  return inode_found;
}

/* 关闭 inode 或减少 inode 的打开数 */
void inode_close(struct inode* inode) {
  /* 若没有进程再打开此文件,将此 inode 去掉并释放空间 */
  enum intr_status old_status = intr_disable();
  if (--inode->i_open_cnts == 0) {
    list_remove(&inode->inode_tag);
    // 确保被释放的是内核内存池
    struct task_struct* cur = runing_thread();
    uint32_t* cur_pagedir_bak = cur->pgdir;
    cur->pgdir = NULL;  // 暂时置为空
    sys_free(inode);
    /*恢复pgdir*/
    cur->pgdir = cur_pagedir_bak;
  }
  intr_set_status(old_status);
}

/*初始化new_inode*/
void inode_init(uint32_t inode_no, struct inode* new_inode) {
  new_inode->i_no = inode_no;
  new_inode->i_size = 0;
  new_inode->i_open_cnts = 0;
  new_inode->write_deny = 0;

  /*初始化块索引数组i_sector*/
  uint8_t sec_idx = 0;
  while (sec_idx < 13) {
    new_inode->i_sectors[sec_idx] = 0;
    sec_idx++;
  }
}