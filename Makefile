BOOTDIR=boot
KernelDIR=kernel
build:
	make -C ${BOOTDIR} image
	make -C ${KernelDIR} image

clean:
	make -C ${BOOTDIR} clean
	make -C ${KernelDIR} clean


run:build
	bochs -qf bochsrc.disk  