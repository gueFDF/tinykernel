%include"boot.inc"
section call_test vstar=0x900
    mov word [addr], near_proc  ;word关键字告诉CPU一次读2字节
    call [addr]     ;间接绝对近调用（内存）
    mov ax, near_proc
    call ax         ;间接寻址（寄存器）
    jmp $
    addr dd 4

near_proc:
    mov ax, 0x1234
  jmp $