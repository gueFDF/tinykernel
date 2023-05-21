DES=kernel.bin
DIR_Image=../boot/boot.img


K=../lib/kernel
K_OBJS=${K}/common.o \
	   ${K}/console.o \
	   ${K}/string.o 
U=../lib/user
U_OBJS=
GCC:
	gcc  -I ../lib/kernel -m32 -c -o main.o main.c 


ld:GCC
	ld -m elf_i386 -T kernel.ld -o ${DES}  main.o ${K_OBJS} ${U_OBJS}
	@-rm -rf *.o
	



image:ld
	dd if=kernel.bin of=${DIR_Image} bs=512 count=200 seek=9 conv=notrunc
	# @-rm -rf *.bin


clean:
	rm -rf *.bin