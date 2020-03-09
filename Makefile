ARCH ?= x86_64
DEBUG ?= 1

NAME = myos

OUTDIR  :=  $(CURDIR)
ISODIR  :=  $(OUTDIR)/iso

TOOLCHAIN_BASE := /usr/local/x86_64-elf-gcc/bin/# To make clion happy

CC      :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-gcc
CPP     :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-g++
AR      :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-ar
OBJCOPY :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-objcopy
LD      :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-ld
NM      :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-nm
BINFILE :=  kernel/kernel.bin  # must be same with kernel/Makefile
ISOFILE :=  $(OUTDIR)/$(NAME).iso

CFLAGS  :=  -c -std=c11 -DKERNEL -DARCH=$(ARCH)
CFLAGS  +=  -Wall -Wextra -Werror=implicit 
CFLAGS  +=  -ffreestanding -ffunction-sections -fdata-sections
ifeq ($(DEBUG), 1)
	CFLAGS  +=  -g -DDEBUG
else
	CFLAGS  +=  -g -DNDEBUG
endif



TEMPLATEFILE := $(CURDIR)/Makefile.template
export CC CPP AR OBJCOPY LD NM CFLAGS TEMPLATEFILE ARCH
# PARAMS  :=  DEBUG=$(DEBUG) ARCH=$(ARCH) CC=$(CC) CPP=$(CPP) AR=$(AR) OBJCOPY=$(OBJCOPY)

build:
	$(MAKE) -C kernel build

clean:
	$(MAKE) -C kernel clean
	rm -rf $(ISOFILE)
	rm -f $(ISODIR)/boot/$(NAME).bin

iso: build
	rm -rf $(ISODIR)
	mkdir -p $(ISODIR)/boot/grub
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	cp $(BINFILE) $(ISODIR)/boot/$(NAME).bin
	grub-mkrescue -o $(ISOFILE) $(ISODIR) 

QEMU := qemu-system-x86_64
QEMUFLAGS := -m 16385 -smp 4 -cdrom $(ISOFILE) -vga vmware -serial stdio -boot order=d

run: iso
	- ps aux | grep $(QEMU) | sed -n "1, 1p" | awk '{print $$2}' | xargs -I {} kill -9 {}
	$(QEMU) $(QEMUFLAGS)

debug: iso
	- ps aux | grep $(QEMU) | sed -n "1, 1p" | awk '{print $$2}' | xargs -I {} kill -9 {}
	$(QEMU) -S -gdb tcp::4444 $(QEMUFLAGS)

debugbackground: iso
	- ps aux | grep $(QEMU) | sed -n "1, 1p" | awk '{print $$2}' | xargs -I {} kill -9 {}
	$(QEMU) -S -gdb tcp::4444 $(QEMUFLAGS) &

dump: build
	objdump -S -d kernel/kernel.elf > dump.txt
