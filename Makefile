BOOTDIR=boot

build:
	make -C ${BOOTDIR} image

clean:
	make -C ${BOOTDIR} clean

run:build
	bochs -f bochsrc.disk  