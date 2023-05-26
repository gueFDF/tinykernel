B=boot
K=kernel
U=user
K_OBJS=$K/main.o \
	   $K/common.o \
	   $K/console.o \
	   $K/string.o \
	   $K/types.o \
	   $K/init.o \
	   $K/interrupt.o\
	   $K/timer.o
	
	   
GCC_FLAGS = -c -Wall -m32 -ggdb  \
-nostdinc -fno-pic -fno-builtin -fno-stack-protector


build:${K_OBJS}
	nasm -I $B/include -o $B/mbr.bin   $B/mbr.asm 
	nasm -I $B/include -o $B/loader.bin $B/loader.asm 
	nasm -f elf -o $K/kernel.o $K/kernel.asm 
	nasm -f elf -o $K/print.o $K/print.asm 
	ld -m elf_i386 -T kernel.ld -o kernel.bin ${K_OBJS} $K/kernel.o $K/print.o 

$K/%.o:$K/%.c
	gcc -I $K ${GCC_FLAGS} -o $@ $^ 


dd: build
	dd if=/dev/zero of=$B/boot.img bs=512 count=61440
	dd if=$B/mbr.bin of=$B/boot.img bs=512 count=1 conv=notrunc
	dd if=$B/loader.bin of=$B/boot.img bs=512 count=4 seek=2 conv=notrunc
	dd if=kernel.bin of=$B/boot.img bs=512 count=200 seek=9 conv=notrunc


run:dd
	bochs -qf bochsrc.disk  

clean:
	rm -rf  build 
	rm -rf  kernel/*.o
	rm -rf  boot/*.bin
	rm -rf  *.bin
