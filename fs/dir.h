#ifndef FS_DIR
#define FS_DIR
#include "fs.h"
#include "inode.h"
#include "types.h"
#define MAX_FILE_NAME_LEN 16  // 最大文件名长度

/*目录结构*/
struct dir {
  struct inode* inode;
  uint32_t dir_pos;      // 记录在目录内的偏移
  uint8_t dir_buf[512];  // 目录的数据缓存
};

/*目录项结构*/
struct dir_entry {
  char filename[MAX_FILE_NAME_LEN];  // 文件名称
  uint32_t i_no;                     // 文件对应的inode编号
  enum file_types f_type;            // 文件类型
};

#endif /* FS_DIR */
