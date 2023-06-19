#include "file.h"

#include "dir.h"
#include "inode.h"
#include "stdio_kernel.h"
#include "string.h"
#include "super_block.h"

/*文件表*/
struct file file_table[MAX_FILE_OPEN];

/*分配一个节点*/
int32_t inode_bitmap_alloc(struct partition* part) {
  int32_t bit_idx = bitmap_scan(&part->inode_bitmap, 1);
  if (bit_idx == -1) {
    return -1;
  }
  bitmap_set(&part->inode_bitmap, bit_idx, 1);
  return bit_idx;
}

// 分配一个扇区，返回其扇区地址
int32_t block_bitmap_alloc(struct partition* part) {
  int32_t bit_idx = bitmap_scan(&part->block_bitmap, 1);
  if (bit_idx == -1) {
    return -1;
  }
  bitmap_set(&part->block_bitmap, bit_idx, 1);
  return (part->sb->data_start_lba + bit_idx);
}

/*从文件表file_table中获取一个空闲位，成功返回下标，失败返回-1*/
int32_t get_free_slot_in_global(void) {
  uint32_t fd_idx = 3;
  while (fd_idx < MAX_FILE_OPEN) {
    if (file_table[fd_idx].fd_inode == NULL) {
      break;
    }
    fd_idx++;
  }
  if (fd_idx == MAX_FILE_OPEN) {
    printk("exceed max open files\n");
    return -1;
  }
  return fd_idx;
}

/*将内存中bitmap第bit_idx位所在的512字节同步到硬盘*/
void bitmap_sync(struct partition* part, uint32_t bit_idx, uint8_t btmp) {
  uint32_t off_sec = bit_idx / 4096;         // 该位扇区偏移量
  uint32_t off_size = off_sec * BLOCK_SIZE;  // 该位字节偏移量

  uint32_t sec_lba;
  uint8_t* bitmap_off;
  /*需要被同步到硬盘的位图只有inode_bitmap和block_map*/
  switch (btmp) {
    case INODE_BITMAP:
      sec_lba = part->sb->inode_bitmap_lba + off_sec;
      bitmap_off = part->inode_bitmap.bits + off_size;
      break;
    case BITMAP_MASK:
      sec_lba = part->sb->block_bitmap_lba + off_sec;
      bitmap_off = part->block_bitmap.bits + off_size;
      break;
  }
  ide_write(part->my_disk, sec_lba, bitmap_off, 1);
}

int32_t file_create(struct dir* parent_dir, char* filename, uint8_t flag) {
  /*后续操作的公共缓冲区*/
  void* io_buf = sys_malloc(1204);
  if (io_buf == NULL) {
    printk("in file_creat: sys_malloc for io_buf failed\n");
    return -1;
  }

  uint8_t rollback_step = 0;  // 用于回滚操作状态

  /*为新文件分配inode*/
  int32_t inode_no = inode_bitmap_alloc(cur_part);
  if (inode_no == -1) {
    printk("in file_creat: allocate inode failed\n");
    return -1;
  }

  /* 此 inode 要从堆中申请内存,不可生成局部变量(函数退出时会释放)
   * 因为 file_table 数组中的文件描述符的 inode 指针要指向它 */
  struct inode* new_file_inode =
      (struct inode*)sys_malloc(sizeof(struct inode));
  if (new_file_inode == NULL) {
    printk("file_create: sys_malloc for inode failded\n");
    rollback_step = 1;
    goto rollback;
  }
  inode_init(inode_no, new_file_inode);  // 初始化inode节点

  int fd_idx = get_free_slot_in_global();
  if (fd_idx == -1) {
    printk("exceed max open files\n");
    rollback_step = 2;
    goto rollback;
  }

  file_table[fd_idx].fd_inode = new_file_inode;
  file_table[fd_idx].fd_pos = 0;
  file_table[fd_idx].fd_flag = flag;
  file_table[fd_idx].fd_inode->write_deny = false;
  struct dir_entry new_dir_entry;
  memset(&new_dir_entry, 0, sizeof(struct dir_entry));
  create_dir_entry(filename, inode_no, FT_REGULAR, &new_dir_entry);

  /*同步内存数据到磁盘*/
  if (!sync_dir_entry(parent_dir, &new_dir_entry, io_buf)) {
    printk("sync dir_entry to disk failed\n");
    rollback_step = 3;
    goto rollback;
  }

  memset(io_buf, 0, 1024);
  /*将父目录的i节点的内容同步到硬盘*/
  inode_sync(cur_part, parent_dir->inode, io_buf);

  memset(io_buf, 0, 1024);
  /*将新创建文件的 i 结点内容同步到硬盘*/
  inode_sync(cur_part, new_file_inode, io_buf);

  /*将inode_bitmap位图同步到硬盘*/
  bitmap_sync(cur_part, inode_no, INODE_BITMAP);

  /*将创建的文件inode节点添加到open_inodes链表*/

  list_push(&cur_part->open_inodes, &new_file_inode->inode_tag);
  new_file_inode->i_open_cnts = 1;
  sys_free(io_buf);

  return pcb_fd_install(fd_idx);
rollback:
  switch (rollback_step) {
    case 3:
      memset(&file_table[fd_idx], 0, sizeof(struct file));
    case 2:
      sys_free(new_file_inode);
    case 1:
      bitmap_set(&cur_part->inode_bitmap, inode_no, 0);
      break;
  }
  sys_free(io_buf);
  return -1;
}
