#ifndef KERNEL_GLOBAL
#define KERNEL_GLOBAL
#include "types.h"

// RPL（四个权级）
#define RPL0 0
#define RPL1 1
#define RPL2 2
#define RPL3 3

// TI(全局描述符表/局部描述符表)
#define TI_GDT 0
#define TI_LDT 1

// 选择子定义(内核)
#define SELECTOR_K_CODE ((1 << 3) + (TI_GDT << 2) + RPL0)  // 代码段选择子
#define SELECTOR_K_DATA ((2 << 3) + (TI_GDT << 2) + RPL0)  // 数据段选择子
#define SELECTOR_K_STACK SELECTOR_K_DATA  // 栈段选择子(和数据段共用一个段)
#define SELECTOR_K_GS ((3 << 3) + (TI_GDT << 2) + RPL0)  // 显卡段选择自

//--------------   IDT描述符属性  ------------
#define IDT_DESC_P 1  // P标识位(和GDT描述符一样，段存在在内存中为1)
#define IDT_DESC_DPL0 0  // 权级，因为中断是从用户到内核，所以只设计两个特权级
#define IDT_DESC_DPL3 3
#define IDT_DESC_32_TYPE 0xE  // 32位的门(1110)
#define IDT_DESC_16_TYPE 0x6  // 16位的门(0110)，不用，定义它只为和32位门区分
#define IDT_DESC_ATTR_DPL0 \
  ((IDT_DESC_P << 7) + (IDT_DESC_DPL0 << 5) + IDT_DESC_32_TYPE)
#define IDT_DESC_ATTR_DPL3 \
  ((IDT_DESC_P << 7) + (IDT_DESC_DPL3 << 5) + IDT_DESC_32_TYPE)


#define NULL ((void*)0)
#define bool int
#define true 1
#define false 0


#endif /* KERNEL_GLOBAL */
