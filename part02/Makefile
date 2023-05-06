.PHONY:run

OBJ=mbr.bin

build:
	nasm mbr.S -o $(OBJ)

image: build
	dd if=/dev/zero of=diska.img bs=512 count=61440
	dd if=$(OBJ) of=boot.img bs=512 count=1
	dd if=diska.img of=boot.img skip=1 seek=1 bs=512 count=61439

run: image
	bochs -f bochsrc.disk 

clean:
	rm -rf *.img $(OBJ) *.out *.lock