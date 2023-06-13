B=boot
K=kernel
D=device
T=thread
U=userprog
L=lib

LU=lib/user
LK=lib/kernel


K_OBJS=$K/main.o \
	   $K/common.o \
	   $K/console.o \
	   $K/string.o \
	   $K/types.o \
	   $K/init.o \
	   $K/interrupt.o\
	   $K/debug.o \
	   $K/bitmap.o \
	   $K/memory.o \
	   $K/list.o \
	   $K/print.o

D_OBJS=$D/keyboard.o \
       $D/timer.o  \
	   $D/ioqueue.o \
	   $D/ide.o
	   

T_OBJS=$T/sync.o \
	   $T/thread.o 


U_OBJS=$U/tss.o \
	   $U/process.o \
	   $U/syscall_init.o

L_OBJS=$L/stdio.o

LU_OBJS=${LU}/syscall.o

LK_OBJS=${LK}/stdio_kernel.o 
	
GCC_FLAGS = -c -Wall -m32 -ggdb  \
-nostdinc -fno-pic -fno-builtin -fno-stack-protector

OBJS=${K_OBJS}   \
	 ${D_OBJS}   \
	 ${T_OBJS}   \
	 ${U_OBJS}   \
	 ${LK_OBJS}  \
	 ${LU_OBJS}  \
	 ${L_OBJS}

include= -I $K -I $D -I $T -I $U -I $L -I ${LK} -I ${LU} 

build:${OBJS}
	nasm -I $B/include -o $B/mbr.bin   $B/mbr.asm 
	nasm -I $B/include -o $B/loader.bin $B/loader.asm 
	nasm -f elf -o $K/kernel.o $K/kernel.asm 
	nasm -f elf -o $T/switch.o $T/switch.asm
	ld -m elf_i386 -T kernel.ld -o kernel.bin ${OBJS} $K/kernel.o  $T/switch.o

$K/%.o:$K/%.c 
	gcc ${include} ${GCC_FLAGS} -o $@ $^ 

$D/%.o:$D/%.c
	gcc ${include} ${GCC_FLAGS} -o $@ $^ 

$T/%.o:$T/%.c
	gcc ${include} ${GCC_FLAGS} -o $@ $^ 

$U/%.o:$U/%.c
	gcc ${include} ${GCC_FLAGS} -o $@ $^

$L/%.o:$L/%.c
	gcc ${include} ${GCC_FLAGS} -o $@ $^
	
${LK}/%.o:${LK}/%.c
	gcc ${include} ${GCC_FLAGS} -o $@ $^

${LU}/%.o:${LU}/%.c
	gcc ${include} ${GCC_FLAGS} -o $@ $^



dd: build
	dd if=/dev/zero of=$B/boot.img bs=512 count=61440
	dd if=$B/mbr.bin of=$B/boot.img bs=512 count=1 conv=notrunc
	dd if=$B/loader.bin of=$B/boot.img bs=512 count=4 seek=2 conv=notrunc
	dd if=kernel.bin of=$B/boot.img bs=512 count=200 seek=9 conv=notrunc


run:dd
	bochs -qf bochsrc.disk  
	
clean:
	@ rm -rf  build 
	@ rm -f  kernel/*.o
	@ rm -f  device/*.o
	@ rm -f  thread/*.o
	@ rm -f  userprog/*.o
	@ rm -f  boot/*.bin
	@ rm -f  *.bin
	@ rm -f  boot/*.img
	@ rm -rf lib/user/*.o
	@ rm -rf lib/kernel/*.o
	@ rm -rf lib/*.o
