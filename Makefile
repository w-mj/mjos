ARCH ?= x86_64
DEBUG ?= 1

NAME = mjos

OUTDIR  :=  $(CURDIR)
ISODIR  :=  $(OUTDIR)/iso
SYSROOT :=  $(CURDIR)/sysroot

TOOLCHAIN_BASE := $(CURDIR)/opt/bin/# To make clion happy

CC      :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-gcc
CXX     :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-g++
AR      :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-ar
OBJCOPY :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-objcopy
LD      :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-ld
NM      :=  $(TOOLCHAIN_BASE)$(ARCH)-elf-nm
BINFILE :=  kernel/kernel.bin  # must be same with kernel/Makefile
ISOFILE :=  $(OUTDIR)/$(NAME).iso

CFLAGS :=
CXXFLAGS :=

TESTINC := -I$(CURDIR)/tools/UnitTest -I/usr/include

ifeq ($(DEBUG), 1)
	CFLAGS  +=  -g -DDEBUG
	CXXFLAGS  +=  -g -DDEBUG
else
	CFLAGS  +=  -g -DNDEBUG
	CXXFLAGS  +=  -g -DDEBUG
endif



TEMPLATEFILE := $(CURDIR)/Makefile.template

export CC CXX AR OBJCOPY LD NM CFLAGS TEMPLATEFILE ARCH CXXFLAGS TESTINC DISKDIR NAME TOOLCHAIN_BASE SYSROOT

all: build

build: build-kernel build-user

test:  test-kernel test-user

clean:

build-kernel: build-user
	$(MAKE) -C kernel build

test-kernel: FORCE
	$(MAKE) -C kernel test

clean-kernel: FORCE
	$(MAKE) -C kernel clean

build-user: mksysroot FORCE
	mkdir -p $(SYSROOT)/usr/bin
	mkdir -p mnt
	$(MAKE) -C user build
	- ps aux | grep $(QEMU) | sed -n "1, 1p" | awk '{print $$2}' | xargs -I {} kill -9 {}
	- guestunmount mnt
	guestmount -a disk.img -m /dev/sda mnt
	rsync -a $(SYSROOT)/* mnt
	guestunmount mnt
	rm -rf mnt

test-user: FORCE
	$(MAKE) -C user test

clean-user: FORCE
	$(MAKE) -C user clean

FORCE:

clean:  clean-kernel clean-user
	rm -rf $(ISOFILE)
	rm -f $(ISODIR)/boot/$(NAME).bin

iso: build
	rm -rf $(ISODIR)
	mkdir -p $(ISODIR)/boot/grub
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	cp $(BINFILE) $(ISODIR)/boot/$(NAME).bin
	grub-mkrescue -o $(ISOFILE) $(ISODIR) 

QEMU := qemu-system-x86_64
QEMUFLAGS := -m 16385 -smp 4 -cdrom $(ISOFILE) -vga vmware -serial stdio -boot order=d \
 			 -drive id=disk,file=disk.img,if=none,format=raw \
 			 -device ahci,id=ahci \
 			 -device ide-drive,drive=disk,bus=ahci.0

run: iso disk
	- ps aux | grep $(QEMU) | sed -n "1, 1p" | awk '{print $$2}' | xargs -I {} kill -9 {}
	$(QEMU) $(QEMUFLAGS) | tee log.txt

debug: iso disk
	- ps aux | grep $(QEMU) | sed -n "1, 1p" | awk '{print $$2}' | xargs -I {} kill -9 {}
	$(QEMU) -S -gdb tcp::4444 $(QEMUFLAGS) | tee log.txt

debugbackground: iso disk
	- ps aux | grep $(QEMU) | sed -n "1, 1p" | awk '{print $$2}' | xargs -I {} kill -9 {}
	$(QEMU) -S -gdb tcp::4444 $(QEMUFLAGS) > log.txt 2>&1 &

dump: build-kernel
	objdump -S -d kernel/kernel.elf > dump.txt

disk: disk.img

disk.img:
	dd if=/dev/zero of=disk.img bs=1024 count=1024
	mkfs.ext2 disk.img

makecompiledatabase:
	compiledb -nf make
	compiledb -n  make test

mksysroot:
	mkdir -p  sysroot/usr/include/mjos
	rsync -a  kernel/include/* sysroot/usr/include/
	cp        kernel/libc/include/sys/kstat.h sysroot/usr/include/sys/kstat.h
	rsync -a  kernel/arch/$(ARCH)/include/* sysroot/usr/include/
	# find $(SYSROOT)/usr/include/mjos/ -name '*.h' | awk '{print "#include \""$$0"\""}' > sysroot/usr/include/mjos.h

# Disable implicit rules
.SUFFIXES:
SUFFIXES :=
%.sh:
.sh:
