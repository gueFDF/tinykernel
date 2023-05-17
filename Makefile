BOOTDIR=boot
KernelDIR=kernel
Lib_kernel=lib/kernel
build:
	
	make -C ${Lib_kernel} build
	make -C ${BOOTDIR} image
	make -C ${KernelDIR} image
	make -C ${Lib_kernel} clean

clean:
	make -C ${BOOTDIR} clean
	make -C ${KernelDIR} clean
	make -C ${Lib_kernel} clean


run:build
	bochs -qf bochsrc.disk  