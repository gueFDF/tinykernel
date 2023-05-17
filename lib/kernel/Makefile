.PHONY:	
SRC_O=print.o

build:${SRC_O}
	
%.o:%.S
	nasm -f elf -o $@ $^
	
clean:
	rm -rf *o 