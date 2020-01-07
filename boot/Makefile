
CC_PREFIX=i686-elf


all: myos.bin

myos.bin: kernel.o boot.o linker.ld
	$(CC_PREFIX)-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

kernel.o: kernel.c
	$(CC_PREFIX)-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

boot.o: boot.S
	$(CC_PREFIX)-as boot.S -o boot.o

run: all
	qemu-system-x86_64 -kernel myos.bin

iso: all
	rm -rf iso/
	mkdir -p iso/boot/grub
	cp myos.bin iso/boot/myos.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso iso

clean: 
	-rm -f *.o *.bin *.iso
	-rm -rf iso/

.PHONY: clean


