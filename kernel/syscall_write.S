section .data 
str_c_lib: db "c library says: hello world!", 0xa ;0xa为LF
str_c_lib_len equ $-str_c_lib
str_syscall: db "syscall says: hello world!", 0xa
str_syscall_len equ $-str_syscall

section .text
global _start
_start:
 ;-------------------- 方式 1: 模拟 C 语言中系统调用库函数 write----------------
    push str_c_lib_len  ;压入参数
    push str_c_lib
    push 1

    call simu_write 
    add esp, 12 ;回收栈空间

 ;-------------------- 方式 2: 跨过库函数,直接进行系统调用 ----------------
    mov eax, 4   ;第 4 号子功能是 write 系统调用(不是 C 库函数 write)
    mov ebx, 1
    mov ecx, str_syscall
    mov edx, str_syscall_len
    int 0x80     ;发起中断,通知 Linux 完成请求的功能  


 ;--------------------程序退出 ----------------------
    mov eax,1    ;第 1 号子功能是 exit
    int 0x80     ;发起中断,通知 Linux 完成请求的功能
 



;------------下面自定义的 simu_write 用来模拟 C 库中系统调用函数 write-----------
;这里模拟它的实现原理
simu_write:
    push ebp ;备份ebp
    mov ebp, esp
    mov eax, 4
    mov ebx, [ebp+8]
    mov ecx, [ebp+12]
    mov edx, [ebp+16]
    int 0x80
    mov esp, ebp
    pop ebp
    ret

