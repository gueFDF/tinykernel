;主引导程序
SECTION MBR vstart=0x7c00  ;指定段地址(引导程序的起始地址)
   ;下面几行是使用cs寄存器初始化其他段寄存器，此时cs寄存器为0, 因为jmp 0: 0x7c00
   mov ax,cs      
   mov ds,ax
   mov es,ax
   mov ss,ax
   mov fs,ax                
   ;下面两行保存显卡地址(0xb800)到gs，ax起跳板作用，因为是段首地址所以要左移一位
   mov ax,0xb800
   mov gs,ax
   mov sp,0x7c00      ;初始化栈指针     

 ;功能号(上卷屏幕) AH = 0x06 ,上卷行数 AL=0(0表示全部)（ax=ah+al）
   mov ax,600h       
 ;BH上卷行属性
   mov bx,700h
 ;(CL,CH)左上角位置         
   mov cx,0
 ;(DL,DH)右上角位置发
   mov dx,184fh 
 ;触发中断，执行功能(无返回值)
   int 10h    

  ;直接利用显卡地址打印"  HELLO YR  "
  ; A 表示绿色背景闪烁,4 表示前景色为红色
  mov byte [gs:0x00],' '
  mov byte [gs:0x01],0xA4
  mov byte [gs:0x02],' '
  mov byte [gs:0x03],0xA4
  mov byte [gs:0x04],'H'
  mov byte [gs:0x05],0xA4
  mov byte [gs:0x06],'E'
  mov byte [gs:0x07],0xA4
  mov byte [gs:0x08],'L'
  mov byte [gs:0x09],0xA4
  mov byte [gs:0x0A],'L'
  mov byte [gs:0x0B],0xA4
  mov byte [gs:0x0C],'O'
  mov byte [gs:0x0D],0xA4
  mov byte [gs:0x0E],' '
  mov byte [gs:0x0F],0xA4
  mov byte [gs:0x10],'Y'
  mov byte [gs:0x11],0xA4
  mov byte [gs:0x12],'R'
  mov byte [gs:0x13],0xA4
  mov byte [gs:0x14],' '
  mov byte [gs:0x15],0xA4
  mov byte [gs:0x16],' '
  mov byte [gs:0x17],0xA4


 ;($-$$)指的是段首到该行位置的偏移量 ，这一行代码的目的是将510字节中剩余的用0填满
  times 510-($-$$) db 0
 ;使用魔术标记该扇区为引导扇区
   db 0x55,0xaa