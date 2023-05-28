#include "memory.h"

#include "bitmap.h"
#include "console.h"

#define PG_SIZE 4096  // 一个物理页的大小

// 位图地址(一个4kb位图可以支持128MB内存)，9a000~9e000,4个页框大小的位图
#define MEM_BITMAP_BASE 0xc009a000

#define K_HEAP_START 0xc0100000

typedef struct pool {
  struct bitmap pool_bitmap;  // 本内存池用到的位图结构,用于管理物理内存
  uint32_t phy_addr_start;  // 本内存池所管理物理内存的起始地址
  uint32_t pool_size;       // 本内存池字节容量
} pool;

pool kernel_pool, user_pool;       // 生成内核内存池和用户内存池
struct virtual_addr kernel_vaddr;  // 此结构用来给内核分配虚拟地址

/*初始化内存池*/
static void mem_pool_init(uint32_t all_mem) {
  console_write(" mem_pool_init start\n");
  //(一个目录表+第一个物理页+第 769~1022 个页目录项共指向 254 个页表=256)
  uint32_t page_table_size = PG_SIZE * 256;
  // 已经使用的内存，低端1M+已经映射的页表占用的
  uint32_t used_mem = page_table_size + 0x100000;
  uint32_t free_mem = all_mem - used_mem;

  uint16_t all_free_pages = free_mem / PG_SIZE;
  uint16_t kernel_free_pages = all_free_pages / 2;
  uint16_t user_free_pages = all_free_pages - kernel_free_pages;

  // 取整，不算余数(简化bitmap操作，但会造成内存浪费[余数不会算进内存]，好处是不用担心越界)
  uint32_t kbm_length = kernel_free_pages / 8;  // Kernel BitMap 的长度
  uint32_t ubm_length = user_free_pages / 8;    // User BitMap 的长度

  uint32_t kp_start = used_mem;  // 内核起始地址
  uint32_t up_start = kp_start + kernel_free_pages * PG_SIZE;  // 用户起始地址

  kernel_pool.phy_addr_start = kp_start;
  kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
  kernel_pool.pool_bitmap.btmp_bytes_len = kbm_length;

  user_pool.phy_addr_start = up_start;
  user_pool.pool_size = user_free_pages * PG_SIZE;
  user_pool.pool_bitmap.btmp_bytes_len = ubm_length;

  kernel_pool.pool_bitmap.bits = (void*)MEM_BITMAP_BASE;
  user_pool.pool_bitmap.bits = uint32ToVoidptr(MEM_BITMAP_BASE + kbm_length);

  /*输出内存池信息*/
  console_write("  kernel_pool_bitmap_start:");
  console_write_hex(voidptrTouint32((void*)(kernel_pool.pool_bitmap.bits)));
  console_write("  kernel_pool_phy_addr_start:");
  console_write_hex(kernel_pool.phy_addr_start);
  console_write("\n");

  console_write("  user_pool_bitmap_start:");
  console_write_hex(voidptrTouint32((void*)(user_pool.pool_bitmap.bits)));
  console_write("  user_pool_phy_addr_start:");
  console_write_hex(user_pool.phy_addr_start);
  console_write("\n");

  // 初始化位图
  bitmap_init(&kernel_pool.pool_bitmap);
  bitmap_init(&kernel_pool.pool_bitmap);

  // 初始化内核虚拟地址的位图
  kernel_vaddr.vaddr_bitmap.btmp_bytes_len = kbm_length;
  kernel_vaddr.vaddr_bitmap.bits =
      uint32ToVoidptr(MEM_BITMAP_BASE + kbm_length + ubm_length);
  kernel_vaddr.vaddr_start = K_HEAP_START;
  bitmap_init(&kernel_vaddr.vaddr_bitmap);
  console_write("  kernel_vaddr_bitmap_start:");
  console_write_hex(voidptrTouint32((void*)(kernel_vaddr.vaddr_bitmap.bits)));
  console_write("\n");
  
  console_write(" mem_pool_init done\n");
}

void mem_init(void) {
  console_write("mem_init start\n");
  uint32_t mem_bytes_total = (*(uint32_t*)(0xb03));
  mem_pool_init(mem_bytes_total);  // 初始化内存池
  console_write("mem_init done\n");
}