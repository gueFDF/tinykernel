#include "fs.h"

#include "ide.h"
#include "inode.h"
#include "types.h"
/*格式化分区数据，也就是初始化分区的元数据，创建文件系统*/

static void partition_format(struct disk* hd, struct partition* part) {
  /* blocks_bitmap_init(为方便实现,一个块大小是一扇区)*/
  // 计算每一个模块所要占用的扇区数
  uint32_t boot_sector_sects = 1;
  uint32_t super_block_sects = 1;
  uint32_t inode_bitmap_sects =
      DIV_ROUND_UP(MAX_FILES_PER_PART, BITS_PER_SECTOR);

  uint32_t inode_table_sects =
      DIV_ROUND_UP((sizeof(struct inode) * MAX_FILES_PER_PART), SECTOR_SIZE);

  uint32_t used_sects = boot_sector_sects + super_block_sects +
                        inode_bitmap_sects + inode_table_sects;
  uint32_t free_sects = part->sec_cnt - used_sects;

  /************** 简单处理块位图占据的扇区数 ***************/
  /*块位图是放在可用区域内的*/
  uint32_t block_bitmap_sects;
  block_bitmap_sects = DIV_ROUND_UP(free_sects, BITS_PER_SECTOR);
  uint32_t block_bitmap_bit_len =
      free_sects - block_bitmap_sects;  // 除去块位图占用的扇区

  // 二次计算
  block_bitmap_sects = DIV_ROUND_UP(block_bitmap_bit_len, BITS_PER_SECTOR);
}