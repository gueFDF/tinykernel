#ifndef FS_FS
#define FS_FS
#include "ide.h"
#include "types.h"
// 每个分区所支持最大创建的文件数
#define MAX_FILES_PER_PART 4096

#define BITS_PER_SECTOR 4096  // 每个扇区的位数(512*8)

#define SECTOR_SIZE 512         // 扇区字节大小
#define BLOCK_SIZE SECTOR_SIZE  // 块字节大小
/*文件结构*/
struct file {
  uint32_t fd_pos;  // 当前文件操作的偏移地址，-1表示最大偏移量
  uint32_t fd_flag;  // 文件操作标识
  struct inode* fd_inode;
};
/*标准输入输出描述符*/
enum std_fd {
  stdin_no,   // 0标准输入
  stdout_no,  // 1标准输出
  stderr_no   // 2标准错误
};

/*位图类型*/
enum bitmap_type {
  INODE_BITMAP,  // inode位图
  BLOCK_BITMAP   // 块位图
};

#define MAX_FILE_OPEN 32  // 系统可打开的最大文件数
/*文件类型*/
enum file_types {
  FT_UNKNOWN,   // 不支持的文件类型
  FT_REGULAR,   // 普通文件
  FT_DIRECTORY  // 目录
};

extern struct partition* cur_part;

void filesys_init();
int32_t get_free_solt_in_global();
int32_t inode_bitmap_alloc(struct partition* part);
int32_t block_bitmap_alloc(struct partition* part);
void bitmap_sync(struct partition* part, uint32_t bit_idx, uint8_t btmp);
#endif /* FS_FS */
