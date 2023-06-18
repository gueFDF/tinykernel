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

bool search_dir_entry(struct partition* part, struct dir* pdir,
                      const char* name, struct dir_entry* dir_e);

struct dir* dir_open(struct partition* part, uint32_t inode_no);

void open_root_dir(struct partition* part);

void create_dir_entry(char* filename, uint32_t inode_no, uint8_t file_types,
                      struct dir_entry* p_de);
#endif /* FS_DIR */
