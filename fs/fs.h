#ifndef FS_FS
#define FS_FS
#include "ide.h"
#include "stdint.h"
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

/*文件读写位置偏移*/
enum whence {
  SEEK_SET = 1,
  SEEK_CUR,
  SEEK_END,
};

/* 用来记录查找文件过程中已找到的上级路径,
也就是查找文件过程中“走过的地方” */
struct path_search_record {
  char searched_path[MAX_PATH_LEN];

  struct dir* parent_dir;  // 文件或目录所在的直接父目录
  // 找到的是普通文件,还是目录,找不到将为未知类型(FT_UNKNOWN)
  enum file_types file_type;
};

/*文件属性*/
struct stat {
  uint32_t st_ino;              // inode编号
  uint32_t st_size;             // 尺寸
  enum file_types st_filetype;  // 文件类型
};

extern struct partition* cur_part;

void filesys_init();
char* path_parse(char* pathname, char* name_store);
int32_t file_create(struct dir* parent_dir, char* filename, uint8_t flag);
int32_t path_depth_cnt(char* pathname);
int32_t sys_open(const char* pathname, uint8_t flags);
int32_t sys_close(int32_t fd);
int32_t sys_write(int32_t fd, const void* buf, uint32_t count);
int32_t sys_read(int32_t fd, void* buf, uint32_t count);
int32_t sys_lseek(int32_t fd, int32_t offset, uint8_t whence);
int32_t sys_unlink(const char* pathname);
int32_t sys_mkdir(const char* pathname);
struct dir* sys_opendir(const char* name);
int32_t sys_closedir(struct dir* dir);
struct dir_entry* sys_readdir(struct dir* dir);
void sys_rewinddir(struct dir* dir);
int32_t sys_rmdir(const char* pathname);
char* sys_getcwd(char* buf, uint32_t size);
int32_t sys_chdir(const char* path);
int32_t sys_stat(const char* path, struct stat* buf);
int32_t sys_create(const char* pathname);
#endif /* FS_FS */
