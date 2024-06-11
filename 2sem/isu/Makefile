srcs=$(wildcard *.asm)
outs=$(patsubst %.asm, %, $(srcs))

.PHONY: all clean

all: $(outs)

%.o: %.asm
	nasm -g -f elf32 -o $@ $<

%: %.o
	gcc -dynamic -g -fPIE -o $@ -m32 $<

clean:
	rm -f *.o $(outs)
