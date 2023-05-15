SRC_O=main.o 
DES=kernel.bin
DIR_Image=../boot/boot.img

ld: ${SRC_O}
	ld main.o -Ttext 0xc0001500 -e main -o ${DES}
	@-rm -rf *.o

%.o:%.S
	gcc -c -o $@ $^


image:ld
	dd if=kernel.bin of=${DIR_Image} bs=512 count=200 seek=9 conv=notrunc
	@-rm -rf *.bin


clean:
	rm -rf *.bin