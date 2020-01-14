ARCH ?= x86_64
DEBUG ?= 1

NAME = myos

OUTDIR  :=  $(CURDIR)/out
ISODIR  :=  $(OUTDIR)/iso

CC      :=  $(ARCH)-elf-gcc
AR      :=  $(ARCH)-elf-ar
OBJCOPY :=  $(ARCH)-elf-objcopy

BINFILE :=  $(OUTDIR)/kernel/kernel.bin  # must be same with kernel/Makefile
ISOFILE :=  $(OUTDIR)/$(NAME).iso

PARAMS  :=  DEBUG=$(DEBUG) ARCH=$(ARCH) CC=$(CC) AR=$(AR) OBJCOPY=$(OBJCOPY)

build:
	$(MAKE) -C kernel $(PARAMS) OUTDIR=$(OUTDIR)/kernel build

clean:
	$(MAKE) -C kernel $(PARAMS) OUTDIR=$(OUTDIR)/kernel clean
	rm -rf $(ISOFILE)

iso: build
	rm -rf $(ISODIR)
	mkdir -p $(ISODIR)/boot/grub
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	cp $(BINFILE) $(ISODIR)/boot/$(NAME).bin
	grub-mkrescue -o $(ISOFILE) $(ISODIR) 

run: iso
	qemu-system-x86_64 -accel kvm -smp 4 -m 64 \
	    -cdrom $(ISOFILE) -vga vmware -serial stdio -gdb tcp::4444 -boot order=d
