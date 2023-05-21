.PHONY:	
SRC_O=print.o string.o
GCC_FLAGS = -c -Wall -m32 -ggdb  \
-nostdinc -fno-pic -fno-builtin -fno-stack-protector
INCLUDE=../ 

SRC=$(wildcard *.c)
OBJ=${patsubst %.c,%.o,${SRC}}

build:${OBJ}
	
%.o:%.S
	nasm -f elf -o $@ $^
%.o:%.c
	gcc ${GCC_FLAGS} -I ${INCLUDE}  -o $@ $^ 
	
clean:
	rm -rf *o 