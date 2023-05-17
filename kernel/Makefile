DES=kernel.bin
DIR_Image=../boot/boot.img
file_O=main.o  ../lib/kernel/print.o 
GCC:
	gcc  -I ../lib/kernel -m32 -c -o main.o main.c 


ld:GCC
	ld -m elf_i386 -Ttext 0xc0001500 -e main -o ${DES}  ${file_O} 
	# strip --remove-section=.note.gnu.property ${DES}
	@-rm -rf *.o
	



image:ld
	dd if=kernel.bin of=${DIR_Image} bs=512 count=200 seek=9 conv=notrunc
	# @-rm -rf *.bin


clean:
	rm -rf *.bin