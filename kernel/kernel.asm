[bits 32]
%define ERROR_CODE nop  

%define ZERO push 0

extern idt_table
section .data
global intr_entry_table
intr_entry_table:
%macro VECTOR 2
section .text 
intr%1entry:
    %2
    ;下面进行保存上下文
    push ds
    push es 
    push fs 
    push gs 
    pushad  ;通用寄存器入栈

    ;如果是从片上进入的中断，除了往从片上发送EOI外，还要往主片上发送EOI
    mov al, 0x20    ;中断结束命令 EOI
    out 0xa0, al    ;向从片上发送
    out 0x20, al    ;向主片上发送

    push %1         ;压入中断号，方便后期调试
    call [idt_table+%1*4] ; 调用 idt_table 中的 C 版本中断处理函数
    jmp intr_exit       

section .data  
    dd intr%1entry  ;存储各个中断入口程序的地址
                          ;形成 intr_entry_table 数组
%endmacro

section .text 
global intr_exit 
intr_exit:
    ;恢复上下文
    add esp,4   ;跳过中断号
    popad       ;恢复通用寄存器
    pop gs 
    pop fs 
    pop es 
    pop ds 
    add esp, 4  ;跨过error_code
    iretd       ;从中断返回，32位下等同指令iretd

  VECTOR 0x00, ZERO
  VECTOR 0x01, ZERO
  VECTOR 0x02, ZERO
  VECTOR 0x03, ZERO
  VECTOR 0x04, ZERO
  VECTOR 0x05, ZERO
  VECTOR 0x06, ZERO
  VECTOR 0x07, ZERO
  VECTOR 0x08, ZERO
  VECTOR 0x09, ZERO
  VECTOR 0x0a, ZERO
  VECTOR 0x0b, ZERO
  VECTOR 0x0c, ZERO
  VECTOR 0x0d, ZERO
  VECTOR 0x0e, ZERO
  VECTOR 0x0f, ZERO
  VECTOR 0x10, ZERO
  VECTOR 0x11, ZERO
  VECTOR 0x12, ZERO
  VECTOR 0x13, ZERO
  VECTOR 0x14, ZERO
  VECTOR 0x15, ZERO
  VECTOR 0x16, ZERO
  VECTOR 0x17, ZERO
  VECTOR 0x18, ZERO
  VECTOR 0x19, ZERO
  VECTOR 0x1a, ZERO
  VECTOR 0x1b, ZERO
  VECTOR 0x1c, ZERO
  VECTOR 0x1d, ZERO
  VECTOR 0x1e, ERROR_CODE
  VECTOR 0x1f, ZERO
  VECTOR 0x20, ZERO
  ;因为bochs硬件模拟不全的原因，在此处多添加几个中断
  VECTOR 0x21, ZERO
  VECTOR 0x22, ZERO
  VECTOR 0x23, ZERO
  VECTOR 0x24, ZERO
  VECTOR 0x25, ZERO
  VECTOR 0x26, ZERO
  VECTOR 0x27, ZERO
  VECTOR 0x28, ZERO
  VECTOR 0x29, ZERO
  VECTOR 0x2a, ZERO
  VECTOR 0x2b, ZERO
  VECTOR 0x2c, ZERO
  VECTOR 0x2d, ZERO
  VECTOR 0x2e, ZERO
  VECTOR 0x2f, ZERO
  VECTOR 0x30, ZERO
