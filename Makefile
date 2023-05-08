BOOTDIR=boot

cnf= --enable-gdb-stub
build:
	make -C ${BOOTDIR} image

clean:
	make -C ${BOOTDIR} clean

run:build
	bochs-gdb -f bochsrc.disk  -g ${cnf} 