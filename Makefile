.PHONY:run

OBJ=boot/mbr.bin
INCLUDE=include/

build:
	nasm boot/mbr.S -I ${INCLUDE} -o $(OBJ)
	nasm boot/loader.S -I ${INCLUDE} -o boot/loader.bin

image: build
	dd if=/dev/zero of=boot/diska.img bs=512 count=61440
	dd if=$(OBJ) of=boot/boot.img bs=512 count=1
	dd if=boot/diska.img of=boot/boot.img skip=1 seek=1 bs=512 count=61439
	dd if=boot/loader.bin of=boot/boot.img bs=512 count=1 seek=2 conv=notrunc

run: image
	bochs -f bochsrc.disk 

clean:
	rm -rf *.img $(OBJ) *.out *.lock