#ifndef __KERNEL_INTERRUPT_H
#define __KERNEL_INTERRUPT_H
#include "types.h"

typedef void* intr_handler;  // 用于指向中断处理函数的地址

// 这里用的可编程中断控制器是8259A
#define PIC_M_CTRL 0x20  // 主片的控制端口是0x20
#define PIC_M_DATA 0x21  // 主片的数据端口是0x21
#define PIC_S_CTRL 0xa0  // 从片的控制端口是0xa0
#define PIC_S_DATA 0xa1  // 从片的数据端口是0xa1

#define IDT_DESC_CNT 0x21  // 目前总共支持的中断数(33个)

#define EFLAGS_IF 0x00000200  // eflags寄存器中的IF位为1

/*中断初始化*/
void idt_init();

#endif
