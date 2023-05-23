DES=kernel.bin
DIR_Image=../boot/boot.img
GCC_FLAGS = -c -Wall -m32 -ggdb  \
-nostdinc -fno-pic -fno-builtin -fno-stack-protector
#lib目录下
K=../lib/kernel
K_OBJS=${K}/common.o \
	   ${K}/console.o \
	   ${K}/string.o \
	   ${K}/types.o
	   
U=../lib/user
U_OBJS=

# 当前目录下
SRC=$(wildcard *.c)
OBJ= ./main.o  kernel.o  ./interrupt.o  ./init.o  

GCC: 
	gcc  -I ../lib/kernel -m32 -c -o main.o main.c 



ld:GCC ${OBJ} kernel.o
	ld -m elf_i386 -T kernel.ld -o ${DES} ${OBJ} ${K_OBJS} ${U_OBJS} 
	# @-rm -rf *.o
	

%.o:%.S
	nasm -f elf -o $@ $^
%.o:%.c
	gcc ${GCC_FLAGS} -I ../lib/kernel  -o $@ $^ 

image:ld
	dd if=kernel.bin of=${DIR_Image} bs=512 count=200 seek=9 conv=notrunc
	# @-rm -rf *.bin


clean:
	rm -rf *.bin
	rm -rf *.o