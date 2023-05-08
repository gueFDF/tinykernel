.PHONY:
INCLUDE=../include/
OBJS_BIN=mbr.bin loader.bin



image: ${OBJS_BIN}
	dd if=/dev/zero of=boot.img bs=512 count=61440
	dd if=mbr.bin of=boot.img bs=512 count=1 conv=notrunc
	dd if=loader.bin of=boot.img bs=512 count=1 seek=2 conv=notrunc
	@-rm -rf *.bin

%.bin:%.S
		nasm $^ -I ${INCLUDE} -o $@ 


clean:
	rm -rf *.img  *.out *.lock *.bin 