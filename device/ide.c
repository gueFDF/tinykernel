#include "ide.h"

#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "interrupt.h"
#include "stdio_kernel.h"
#include "timer.h"
/*Command Block
 * registers(用于向硬盘驱动器写入命令字或者从硬盘控制器获得硬盘状态)*/
#define reg_data(channel) (channel->port_base + 0)
#define reg_error(channel) (channel->port_base + 1)
#define reg_sect_cnt(channel) (channel->port_base + 2)  // 存放读写扇区
#define reg_lba_l(channel) (channel->port_base + 3)
#define reg_lba_m(channel) (channel->port_base + 4)
#define reg_lba_h(channel) (channel->port_base + 5)
#define reg_dev(channel) (channel->port_base + 6)
#define reg_status(channel) (channel->port_base + 7)
#define reg_cmd(channel) (reg_status(channel))
/*Control Block registers(控制硬盘工作状态)*/
#define reg_alt_status(channel) (channel->port_base + 0x206)
#define reg_ctl(channel) reg_alt_status(channel)

/* reg_alt_status 寄存器的一些关键位 */
#define BIT_ALT_STAT_BSY 0x80   // 硬盘忙
#define BIT_ALT_STAT_DRDY 0x40  // 驱动器准备好
#define BIT_ALT_STAT_DRQ 0x8    // 数据传输准备好了

/* device 寄存器的一些关键位 */
#define BIT_DEV_MBS 0xa0  // 第 7 位和第 5 位固定为 1
#define BIT_DEV_LBA 0x40  // 第 6 位寻址模式(LBA)
#define BIT_DEV_DEV 0x10  // 第 4 位主盘或从盘(主盘)

/*一些硬盘操作的指令*/
#define CMD_IDENTIFY 0xe0      // identufy指令
#define CMD_READ_SECTOR 0x20   // 读扇区指令
#define CMD_WRITE_SECTOR 0x10  // 写扇区指令

/* 定义可读写的最大扇区数,调试用的 */
#define max_lba ((80 * 1024 * 1024 / 512) - 1)  // 只支持 80MB 硬盘

uint8_t channel_cnt;             // 按硬盘数计算的通道数
struct ide_channel channels[2];  // 有两个ide通道

/*选择的读写的硬盘*/
static void select_disk(struct disk* hd) {
  uint8_t reg_device = BIT_DEV_MBS | BIT_DEV_LBA;  // 固定位和LBA
  if (hd->dev_no == 1) {                           // 判断主从盘
    reg_device |= BIT_DEV_DEV;
  }
  outb(reg_dev(hd->my_channel), reg_device);
}

/* 向硬盘控制器写入起始扇区地址及要读写的扇区数 */
static void select_sector(struct disk* hd, uint32_t lba, uint8_t sec_cnt) {
  ASSERT(lba <= max_lba);
  struct ide_channel* channel = hd->my_channel;
  /*写入要读入的扇区数(0,表示256)*/
  outb(reg_sect_cnt(channel), sec_cnt);

  /*写入lba地址，及扇区号*/
  outb(reg_lba_l(channel), lba);
  outb(reg_lba_m(channel), lba >> 8);
  outb(reg_lba_h(channel), lba >> 16);

  outb(reg_dev(channel), BIT_DEV_MBS | BIT_DEV_LBA |
                             (hd->dev_no == 1 ? BIT_DEV_DEV : 0) | lba >> 24);
}

/* 向通道 channel 发命令 cmd */
static void cmd_out(struct ide_channel* channel, uint8_t cmd) {
  channel->expexting_intr = true;  // 置为true,告诉中断处理程序，正在等待cmd结果
  outb(reg_cmd(channel), cmd);
}

/*硬盘读入sec_cnt个数据到buf*/
static void read_from_sector(struct disk* hd, void* buf, uint8_t sec_cnt) {
  uint32_t size_in_byte;
  if (sec_cnt == 0) {
    size_in_byte = 256 * 512;
  } else {
    size_in_byte = sec_cnt * 512;
  }

  insw(reg_data(hd->my_channel), buf, size_in_byte / 2);
}

/* 将 buf 中 sec_cnt 扇区的数据写入硬盘 */
static void write_to_sector(struct disk* hd, void* buf, uint8_t sec_cnt) {
  uint32_t size_in_byte;
  if (sec_cnt == 0) {
    size_in_byte = 256 * 512;
  } else {
    size_in_byte = sec_cnt * 512;
  }

  outsw(reg_data(hd->my_channel), buf, size_in_byte / 2);
}

/*等待30s(每10ms检查一次)*/
static bool busy_wait(struct disk* hd) {
  struct ide_channel* channel = hd->my_channel;
  uint16_t time_limit = 30 * 1000;
  while (time_limit -= 10 >= 0) {
    if (!(inb(reg_status(channel)) & BIT_ALT_STAT_BSY)) {
      return (inb(reg_status(channel)) & BIT_ALT_STAT_DRQ);
    } else {
      mtime_sleep(10);
    }
  }
  return false;
}

/*从硬盘读取sec_cnt个扇区到buf*/
void ide_read(struct disk* hd, uint32_t lba, void* buf, uint32_t sec_cnt) {
  ASSERT(lba <= max_lba);
  ASSERT(sec_cnt > 0);
  lock_acquire(&hd->my_channel->lock);  // 一个通道同时只允许操作一个磁盘

  /*1.选择操作磁盘*/
  select_disk(hd);
  uint32_t secs_op;        // 每次操作的扇区数
  uint32_t secs_done = 0;  // 已完成的扇区数
  while (secs_done < sec_cnt) {
    if ((secs_done + 256) <= sec_cnt) {
      secs_op = 256;
    } else {
      secs_op = sec_cnt - secs_done;
    }

    /*2.写入待读入的扇区数和起始扇区号*/
    select_sector(hd, lba + secs_done, secs_op);

    /*3.执行的命令写入reg_cmd寄存器*/
    cmd_out(hd->my_channel, CMD_READ_SECTOR);

    sema_down(&hd->my_channel->disk_done);

    /*4.检测硬盘状态是否可读*/
    /*醒来后开始执行下面代码*/
    if (!busy_wait(hd)) {  // 若失败
      char error[64];
      sprintf(error, "%s read sector %d failed!!!!\n", hd->name, lba);
      PANIC(error);
    }

    /*5.把数据从硬盘缓存区读出*/
    read_from_sector(hd, (void*)((uint32_t)buf + secs_done * 512), secs_op);
  }
  lock_release(&hd->my_channel->lock);
}

/*从硬盘读取sec_cnt个扇区到buf*/
void ide_write(struct disk* hd, uint32_t lba, void* buf, uint32_t sec_cnt) {
  ASSERT(lba <= max_lba);
  ASSERT(sec_cnt > 0);
  lock_acquire(&hd->my_channel->lock);  // 一个通道同时只允许操作一个磁盘

  /*1.选择操作磁盘*/
  select_disk(hd);
  uint32_t secs_op;        // 每次操作的扇区数
  uint32_t secs_done = 0;  // 已完成的扇区数
  while (secs_done < sec_cnt) {
    if ((secs_done + 256) <= sec_cnt) {
      secs_op = 256;
    } else {
      secs_op = sec_cnt - secs_done;
    }

    /*2.写入待读入的扇区数和起始扇区号*/
    select_sector(hd, lba + secs_done, secs_op);

    /*3.执行的命令写入reg_cmd寄存器*/
    cmd_out(hd->my_channel, CMD_WRITE_SECTOR);

    /*4.检测硬盘状态是否可读*/
    if (!busy_wait(hd)) {  // 若失败
      char error[64];
      sprintf(error, "%s write sector %d failed!!!!\n", hd->name, lba);
      PANIC(error);
    }

    /*5.把数据写入硬盘缓存*/
    write_to_sector(hd, (void*)((uint32_t)buf + secs_done * 512), secs_op);
    sema_down(&hd->my_channel->disk_done);
    secs_done += secs_op;
  }
  lock_release(&hd->my_channel->lock);
}

/*硬盘中断处理程序*/
void intr_hd_handler(uint8_t irq_no) {
  ASSERT(irq_no == 0x2e || irq_no == 0x2f);
  uint8_t ch_no = irq_no - 0x2e;
  struct ide_channel* channel = &channels[ch_no];
  ASSERT(channel->irq_no == irq_no);
  if (channel->expexting_intr) {
    channel->expexting_intr = false;
    sema_up(&channel->disk_done);
    // 读取状态寄存器使硬盘控制器认为此次的中断已被处理,从而硬盘可以继续执行新的读写
    inb(reg_status(channel));
  }
}
/*硬盘初始化*/
void ide_init() {
  printk("ide_init start\n");
  uint8_t hd_cnt = *((uint8_t*)(0x475));  // 获取硬盘的数量
  ASSERT(hd_cnt > 0);
  channel_cnt = DIV_ROUND_UP(hd_cnt, 2);  // 计算需要的通道数量
  struct ide_channel* channel;
  uint8_t channel_no = 0;

  /*处理每个通道上的硬盘*/
  while (channel_no < channel_cnt) {
    channel = &channels[channel_no];
    sprintf(channel->name, "ide%d", channel_no);
    /*为每一个ide通道初始化端口基址及中断向量*/
    switch (channel_no) {
      case 0:
        channel->port_base = 0x1f0;  // prim通道
        channel->irq_no = 0x20 + 14;
        break;
      case 1:
        channel->port_base = 0x170;  // sec通道
        channel->irq_no = 0x20 + 15;
        break;
    }
    lock_init(&channel->lock);
    seam_init(&channel->disk_done, 0);
    register_handler(channel->irq_no, intr_hd_handler);
    
    channel_no++;
  }

  printk("ide_init done\n");
}