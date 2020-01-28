ARCH ?= x86_64
DEBUG ?= 1

NAME = myos

OUTDIR  :=  $(CURDIR)
ISODIR  :=  $(OUTDIR)/iso

CC      :=  $(ARCH)-elf-gcc
CPP     :=  $(ARCH)-elf-g++
AR      :=  $(ARCH)-elf-ar
OBJCOPY :=  $(ARCH)-elf-objcopy
LD      :=  $(ARCH)-elf-ld
NM      :=  $(ARCH)-elf-nm
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

run: iso
	qemu-system-x86_64 -accel kvm -smp 4 -m 64 \
	    -cdrom $(ISOFILE) -vga vmware -serial stdio -gdb tcp::4444 -boot order=d
