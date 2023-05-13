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

  total_mem_bytes dd 0 ;用来保存内存容量，以字节为单位(计算可得知此处地址为0xb03)
  
  ;初始化GDT_ptr指针(存放在GDTR寄存器)
  ;低16位存放GDT界限，高32位存放起始地址
  gdt_ptr dw GDT_LIMIT
          dd GDT_BASE

  ards_buf times 244 db 0;存放ARDS
  ards_nr dw 0 ;ARDS的数量
  
loader_start:
;---------------进行内存检查---------------
 xor ebx,ebx ;ebx置0
 mov edx, 0x534d4150 ;('SMAP')
 mov di, ards_buf  ;ards结构缓冲区
.e820_mem_get_loop:
  mov eax,0x0000e820 ;0x15的子功能号 ，获取内存容量
  mov ecx,20  ;ARDS 地址范围描述符结构大小是 20 字节
  int 0x15
  jc  .e820_failed_so_try_e801 ;若 cf 位为 1 则有错误发生,尝试 0xe801 子功能
  add di,cx  ;移动执行下一片存放ARDS的缓存
  inc word [ards_nr] ;指定的内存地址中的16位值进行加一操作
  cmp ebx,0  ;若 ebx 为 0 且 cf 不为 1,这说明 ards 全部返回,当前已是最后一个
  jnz .e820_mem_get_loop ;如果不是最后一个，循环继续

  mov cx,[ards_nr] ;先从内存获取ARDS的个数(循环因子，loop遍历次数)
  mov ebx,ards_buf
  xor edx,edx  ;清0后面用来存放最大的内存容量
.find_max_mem_area:
  ;无需判断 type 是否为 1,最大的内存块一定是可被使用的
  ;base_add_low+length_low
  mov eax,[ebx]  ;base_add_low
  add eax,[ebx+8] ;length_low
  add ebx,20  ;移动到下一个ARDS
  cmp edx,eax ;如果edx的值大于或等于eax的值，则标志位寄存器中的ZF（零标志位）会被清除
  jge .next_ards ;如果标志位寄存器中的ZF被清除则跳转
  mov edx,eax ;更新最大值
.next_ards:
  loop .find_max_mem_area
  jmp .mem_get_ok

.e820_failed_so_try_e801:
  mov ax,0xe801
  int 0x15
  jc .e801_failed_so_try88;若当前 e801 方法失败,就尝试 0x88 方法
  ;1.先算出低15MB
  mov cx,0x400 ;1024 单位换算需要用到
  ;因为此处是16位乘法，所以高16为放在dx,低16放在ax,下面是为了获取完整乘积
  mul cx  ;单位换算。ax*cx
  shl edx,16 ;左移16位
  and eax,0x0000FFFF;只保留底16位
  or edx,eax
  add edx,0x100000 ;是获取到的内存总比实际大小少 1MB,故在此“补偿”
  mov esi,edx ;备份

  ;2. 再将 16MB 以上的内存转换为 byte 为单位，寄存器 bx 和 dx 中是以 64KB 为单位的内存数量
  xor eax,eax ;置零
  mov ax,bx
  mov ecx,0x10000 ;1024*64
  mul ecx 
  ;因为此方法只能测出4GB内存，32位寄存器可以存下，所以乘积只有低32位在eax
  add esi,eax ;相加
  mov edx,esi
  jmp .mem_get_ok

.e801_failed_so_try88:
   ;int 15后,ax存入的是以KB为单位的内存容量
  mov ah,0x88
  int 0x15
  jc .error_hlt
  and eax,0x0000FFFF ;只保留低16位
  mov cx, 0x400
  mul cx
  shl edx,16
  or edx,eax
  add edx,0x100000  ;0x88子功能只会返回 1MB 以上的内存,故实际内存大小要加上 1MB

.mem_get_ok:
  mov [total_mem_bytes], edx

;---------------打开保护模式---------------
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
 jmp dword SELECTOR_CODE:p_mode_start

.error_hlt:		      ;出错则挂起
   hlt

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


;-------------   创建页目录及页表   ---------------
setup_page:
;先把页目录占用的空间逐字节清0
  mov ecx,4096
  mov esi,0
.clear_page_dir:
  mov byte [PAGE_DIR_TABLE_POS] ,0
  inc esi
  loop .clear_page_dir

;开始创建也目录项(PDE)
.creat_pde: 
  mov eax,PAGE_DIR_TABLE_POS
  add eax,0x1000  ;此时 eax 为第一个页表的位置及属性
  mov ebx,eax     ;此处为 ebx 赋值,是为.create_pte 做准备,ebx 为基址

  or eax, PG_US_U | PG_RW_W | PG_P
  mov [PAGE_DIR_TABLE_POS+0x0] ,eax ;第一个目录项指向第一个页表项(保证页表开启后的顺利切换)
  mov [PAGE_DIR_TABLE_POS+0xc00],eax;第768个目录项指向第一个页表项
  
  sub eax,0x1000
  mov [PAGE_DIR_TABLE_POS+4092],eax ;使最后一个目录项指向页目录表自己的地址

;下面创建页表项(PTE)
  mov ecx,256   ;1M低端内存 / 每页大小4k =256
  mov esi,0
  mov edx,PG_US_U | PG_RW_W | PG_P
.clear_page_pte:
  mov [ebx+esi*4],edx
  inc esi
  loop .clear_page_pte

;创建内核其他页表的 PDE
  mov eax, PAGE_DIR_TABLE_POS
  add eax,02000 ;此时为第二个页表的位置
  or eax,PG_US_U | PG_RW_W | PG_P
  mov ebx,PAGE_DIR_TABLE_POS
  mov ecx, 254  ; 范围为第 769~1022 的所有目录项数量
  mov esi, 769
.creat_kernel_pde:
  mov [ebx+esi*4],eax
  inc esi
  add eax,0x1000
  loop .creat_kernel_pde
  ret




