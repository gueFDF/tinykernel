#include "console.h"
#include "debug.h"
#include "dir.h"
#include "fs.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
#include "print.h"
#include "process.h"
#include "stdio.h"
#include "stdio_kernel.h"
#include "string.h"
#include "sync.h"
#include "syscall.h"
#include "syscall_init.h"
#include "thread.h"
void k_thread_a(void*);
void k_thread_b(void*);
void u_prog_a(void);
void u_prog_b(void);

int main(void) {
  console_write("I am kernel\n");
  init_all();
  intr_enable();
  process_execute(u_prog_a, "u_prog_a");
  process_execute(u_prog_b, "u_prog_b");
  thread_start("k_thread_a", 31, k_thread_a, "I am thread_a");
  thread_start("k_thread_b", 31, k_thread_b, "I am thread_b");

  sys_mkdir("/dir1");
  sys_mkdir("/dir1/dir2");
  uint32_t fd = sys_open("/dir1/file1", O_CREAT | O_RDWR);
  uint32_t fd2 = sys_open("/dir1/file2", O_CREAT | O_RDWR);
  sys_close(fd);
  sys_close(fd2);
  sys_rmdir("/dir1/dir2");
  struct dir* p_dir = sys_opendir("/dir1");

  if (p_dir) {
    printf("/dir1 open done!\ncontent:\n");
    char* type = NULL;
    struct dir_entry* dir_e = NULL;
    while ((dir_e = sys_readdir(p_dir))) {
      if (dir_e->f_type == FT_REGULAR) {
        type = "regular";
      } else {
        type = "directory";
      }
      printf(" %s %s\n", type, dir_e->filename);
    }
    if (sys_closedir(p_dir) == 0) {
      printf("/dir1 close done!\n");
    } else {
      printf("/dir1 close fail!\n");
    }
  } else {
    printf("/dir1 open fail!\n");
  }
  char cwd_buf[32] = {0};
  printf("path : %s \n", sys_getcwd(cwd_buf, 32));
  sys_chdir("/dir1");
  printf("path : %s \n", sys_getcwd(cwd_buf, 32));
  struct stat obj_stat;
  sys_stat("/dir1", &obj_stat);
  printf("/dir1`s info\n i_no: %d\n size : %d\n filetype : %s\n ",
         obj_stat.st_ino, obj_stat.st_size,
         obj_stat.st_filetype == 2 ? "directory" : "regular");
  while (1)
    ;
  return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
  while (1)
    ;
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {
  while (1)
    ;
}

/* 测试用户进程 */
void u_prog_a(void) {
  while (1)
    ;
}

/* 测试用户进程 */
void u_prog_b(void) {
  while (1)
    ;
}

/*init进程*/
void init(void) {
  uint32_t ret_pid = fork();
  if (ret_pid) {
    printf("i am father, my pid is %d,child pid is %d\n", getpid(), ret_pid);
  } else {
    printf("i am child, my pid is %d, ret pid is %d\n", getpid(), ret_pid);
  }
  while (1)
    ;
}