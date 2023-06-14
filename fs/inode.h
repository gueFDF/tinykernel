#ifndef FS_INODE
#define FS_INODE
#include "global.h"
#include "list.h"
#include "types.h"
/*inode结构*/
struct inode {
  uint32_t i_no;    // inode编号
  uint32_t i_size;  // 文件大小(若为目录，是指目录项大小之和)
  uint32_t i_open_cnts;  // 记录此文件被打开的次数
  bool write_deny;  // 写文件不能并行，进程写文件前检查此标识
  uint32_t i_sectors[13];  // 块的索引表，前12项为直接块，后1项为间接块
  struct list_elem inode_tag;
};

#endif /* FS_INODE */
