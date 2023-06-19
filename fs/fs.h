#ifndef FS_FS
#define FS_FS
#include "ide.h"
#include "types.h"
// 每个分区所支持最大创建的文件数
#define MAX_FILES_PER_PART 4096

#define BITS_PER_SECTOR 4096  // 每个扇区的位数(512*8)

#define SECTOR_SIZE 512         // 扇区字节大小
#define BLOCK_SIZE SECTOR_SIZE  // 块字节大小

#define MAX_PATH_LEN 512  // 路径最大长度

/*文件类型*/
enum file_types {
  FT_UNKNOWN,   // 不支持的文件类型
  FT_REGULAR,   // 普通文件
  FT_DIRECTORY  // 目录
};

/*打开文件的选项*/
enum oflags {
  O_RDINLY,    // 只读
  O_WRONLY,    // 只写
  O_RDWR,      // 读写
  O_CREAT = 4  // 创建
};

/* 用来记录查找文件过程中已找到的上级路径,
也就是查找文件过程中“走过的地方” */
struct path_search_record {
  char searched_path[MAX_PATH_LEN];

  struct dir* parent_dir;  // 文件或目录所在的直接父目录
  // 找到的是普通文件,还是目录,找不到将为未知类型(FT_UNKNOWN)
  enum file_types file_type;
};

extern struct partition* cur_part;

void filesys_init();
int32_t file_create(struct dir* parent_dir, char* filename, uint8_t flag);
int32_t path_depth_cnt(char* pathname);
int32_t sys_open(const char* pathname, uint8_t flags);
#endif /* FS_FS */
