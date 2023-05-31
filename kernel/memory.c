#include "memory.h"

#include "bitmap.h"
#include "console.h"
#include "debug.h"
#include "string.h"

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

// 返回高10位索引
#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)

// 返回中间10位索引
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12)

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

// 在pf标志的虚拟内存池中申请pg_cnt个虚拟页(虚拟地址的空间要保证连续，所以提供该功能)
// 成功返回首地址，失败返回NULL
static void* vaddr_get(enum pool_flags pf, uint32_t pg_cnt) {
  int vaddr_start = 0, bit_idx_start = -1;
  uint32_t cnt = 0;

  if (pf == PF_KERNEL) {
    bit_idx_start = bitmap_scan(&kernel_vaddr.vaddr_bitmap, pg_cnt);
    if (bit_idx_start == -1) {  // 失败
      return NULL;
    }
    while (cnt < pg_cnt) {  // 将寻找到的空位置标记为1
      bitmap_set(&kernel_vaddr.vaddr_bitmap, bit_idx_start + cnt++, 1);
    }
    vaddr_start = kernel_vaddr.vaddr_start + bit_idx_start * PG_SIZE;
  } else {
    // 用户内存池,将来实现用户进程再补充
  }
  return uint32ToVoidptr(vaddr_start);
}

/* 得到虚拟地址 vaddr 对应的 pte 指针*/
uint32_t* pte_ptr(uint32_t vaddr) {
  uint32_t* pte = uint32ToVoidptr(0xffc00000 + ((vaddr & 0xffc00000) >> 10) +
                                  PTE_IDX(vaddr) * 4);
  return pte;
}

/* 得到虚拟地址 vaddr 对应的 pde 指针*/
uint32_t* pde_ptr(uint32_t vaddr) {
  uint32_t* pde = uint32ToVoidptr((0xfffff000) + PDE_IDX(vaddr) * 4);
  return pde;
}

/* 在 m_pool 指向的物理内存池中分配 1 个物理页, *
 * 成功则返回页框的物理地址,失败则返回 NULL */
static void* palloc(struct pool* m_pool) {
  /* 扫描或设置位图要保证原子操作 */
  int bit_idx = bitmap_scan(&m_pool->pool_bitmap, 1);  // 找一个物理页面
  if (bit_idx == -1) {
    return NULL;
  }
  bitmap_set(&m_pool->pool_bitmap, bit_idx, 1);  // 将此位 bit_idx 置 1

  uint32_t page_phyaddr = ((bit_idx * PG_SIZE) + m_pool->phy_addr_start);

  return uint32ToVoidptr(page_phyaddr);
}

/* 页表中添加虚拟地址_vaddr 与物理地址_page_phyaddr 的映射 */
static void page_table_add(void* _vaddr, void* _page_phyaddr) {
  uint32_t vaddr = voidptrTouint32(_vaddr);
  uint32_t page_phyaddr = voidptrTouint32(_page_phyaddr);

  uint32_t* pde = pde_ptr(vaddr);
  uint32_t* pte = pte_ptr(vaddr);

  if (*pde & 0x00000001) {  // 已经存在
    ASSERT(!(*pte & 0x00000001));
    if (!(*pte & 0x00000001)) {
      // 只要是创建页表,pte 就应该不存在,多判断一下放心
      *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    } else {  // 目前应该不会执行到这,因为上面的 ASSERT 会先执行
      PANIC("pte repeat");
      // *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);  //
    }
  } else {  // 也目录项不存在，所以要先创建页目录再创建页表项
            // 页表用到的页框从内核分配
    uint32_t pde_phyaddr = voidptrTouint32(palloc(&kernel_pool));
    *pde = (pde_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    // 将还页框里面的脏数据清零
    // pte高20位指向该pte所在页框的首地址
    void* page_start = uint32ToVoidptr(voidptrTouint32(pte) & 0xfffff000);
    memset(page_start, 0, PG_SIZE);
    ASSERT(!(*pte & 0x00000001));
    *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
  }
}

/* 分配 pg_cnt 个页空间,成功则返回起始虚拟地址,失败时返回 NULL */
void* malloc_page(enum pool_flags pf, uint32_t pg_cnt) {
  ASSERT(pg_cnt > 0 &&
         pg_cnt < 3840);  // 内存大小16MB(用15MB进行约束，3840个页)
  void* vaddr_start = vaddr_get(pf, pg_cnt);
  if (vaddr_start == NULL) {
    return NULL;
  }
  uint32_t vaddr = voidptrTouint32(vaddr_start);
  uint32_t cnt = pg_cnt;

  pool* mem_pool = pf & PF_KERNEL ? &kernel_pool : &user_pool;
  /* 因为虚拟地址是连续的,但物理地址可以是不连续的,所以逐个做映射*/
  while (cnt-- > 0) {
    void* page_phyaddr = palloc(mem_pool);
    if (page_phyaddr == NULL) {
      // 此处失败，失败时要将曾经已经完成的申请的虚拟地址和物理进行回收，将来实现free时完成
      return NULL;
    }
    page_table_add(uint32ToVoidptr(vaddr), page_phyaddr);
    vaddr += PG_SIZE;  // 下一个虚拟页
  }
  return vaddr_start;
}

/* 从内核物理内存池中申请 cnt 页内存,
成功则返回其虚拟地址,失败则返回 NULL */
void* get_kernel_pages(uint32_t pg_cnt) {
  void* vaddr = malloc_page(PF_KERNEL, pg_cnt);
  if (vaddr != NULL) {  // 若分配的地址不为空,将页框清 0(清除脏数据) 后返回
    memset(vaddr, 0, pg_cnt * PG_SIZE);
  }
  return vaddr;
}