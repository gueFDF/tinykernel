%include"boot.inc"
  section loader vstart=LOADER_BASE_ADDR
  LOADER_STACK_TOP equ LOADER_BASE_ADDR
  jmp loader_start
;构建gdt及其内部描述符
  GDT_BASE:  dd 0x00000000  ;第0个段描述符没有用
             dd 0x00000000  
  CODE_DESC: dd 0x0000FFFF  ;代码段描述符
             dd DESC_CODE_HIGH4

  DATA_STACK_DESC: dd 0x0000FFFF ;数据段和桟段描述符
                   dd DESC_DATA_HIGH4
  VIDEO_DESC:  dd 0x80000007   ;显存段描述符 显存文本模式(0xb8000~0xbffff) 0xbffff-0xb8000=0x7fff/4k=7，limit只需要7 
               dd DESC_VIDEO_HIGH4


  GDT_SIZE  equ $-GDT_BASE  ;GDT大小
  GDT_LIMIT equ GDT_SIZE - 1 ;GDT界限(大小-1)
  times 60 dq 0  ;;此处预留60个描述符空位

  ;初始化选择子
  SELECTOR_CODE  equ (0x0001<<3) + TI_GDT +RPL0
  SELECTOR_DATA  equ (0x0002<<3) + TI_GDT +RPL0
  SELECTOR_VIDEO equ (0x0003<<3) + TI_GDT +RPL0

  ;初始化GDT_ptr指针(存放在GDTR寄存器)
  ;低16位存放GDT界限，高32位存放起始地址
  gdt_ptr dw GDT_LIMIT
          dd GDT_BASE
  loadermsg db '2 loader in real.'
  
loader_start:
 mov sp,  LOADER_BASE_ADDR
 mov bp,  loadermsg
 mov cx, 17 ; CX = 字符串长度
 mov ax, 0x1301 ; AH = 13, AL = 01h
 mov bx, 0x001f ; 页号为 0(BH = 0) 蓝底粉红字(BL = 1fh)
 mov dx, 0x1800
 int 0x10 ; 10h 号中断


 ;准备打开保护模式
 ;1 打开 A20(将端口0x92的第1位置1)
 in al,0x92
 or al ,0000_0010B
 out 0x92,al

 ;2 加载 gdt
 lgdt [gdt_ptr]

 ;3 将 cr0 的 pe 位置 1(CR0寄存器的PE位置1)
 mov eax,cr0
 or eax ,0x00000001
 mov cr0,eax
  
 ;利用jmp清空流水线
 jmp  SELECTOR_CODE:p_mode_start

[bits 32] ;开启32位
p_mode_start:
 ;初始化寄存器
  mov ax,SELECTOR_DATA
  mov ds,ax
  mov es,ax
  mov ss,ax
  mov esp,LOADER_STACK_TOP
  mov ax,SELECTOR_VIDEO
  mov gs,ax

  mov byte [gs:160], 'P'

  jmp $
