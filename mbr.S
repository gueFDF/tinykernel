;主引导程序
SECTION MBR vstart=0x7c00  ;指定段地址(引导程序的起始地址)
   ;下面几行是使用cs寄存器初始化其他段寄存器，此时cs寄存器为0, 因为jmp 0: 0x7c00
   mov ax,cs      
   mov ds,ax
   mov es,ax
   mov ss,ax
   mov fs,ax                
   mov sp,0x7c00      ;初始化栈指针     

 ;功能号(上卷屏幕) AH = 0x06 ,上卷行数 AL=0(0表示全部)（ax=ah+al）
   mov ax,0x600       
 ;BH上卷行属性
   mov bx,0x700
 ;(CL,CH)左上角位置         
   mov cx,0
 ;(DL,DH)右上角位置
   mov dx,0x184f 
 ;触发中断，执行功能(无返回值)
   int 0x10    

   mov ah,3
   mov bh,0
   int 0x10

   mov ax ,message
   mov bp,ax

   mov ax,0x1301
   mov bx,0x2
   int 0x10

   jmp $   ;使程序停在此处 $指该行代码地址

 ;定义打印的字符串
   message db "1 MBR"
 ;($-$$)指的是段首到该行位置的偏移量 ，这一行代码的目的是将512字节的用0填满
   times 510-($-$$) db 0
 ;使用魔术标记该扇区为引导扇区
   db 0x55,0xaa