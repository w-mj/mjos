
CC_PREFIX=i686-elf


all: kernel boot linker.ld
	$(CC_PREFIX)-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

kernel: kernel.c
	$(CC_PREFIX)-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

boot: boot.S
	$(CC_PREFIX)-as boot.S -o boot.o

run: all
	qemu-system-x86_64 -kernel myos.bin

clean: 
	-rm -f *.o *.bin

.PHONY: clean


