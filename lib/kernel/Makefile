.PHONY:	
SRC_O=print.o

build:${SRC_O}
	gcc   -m32 -c -o print_str.o print_str.c  
%.o:%.S
	nasm -f elf -o $@ $^
	
clean:
	rm -rf *o 