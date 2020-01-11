ARCH ?= x86_64

NAME = myos

OUTDIR  :=  $(CURDIR)/out
ISODIR  :=  $(OUTDIR)/iso

BINFILE :=  $(OUTDIR)/kernel/$(NAME).bin  # must be same with kernel/Makefile
ISOFILE :=  $(OUTDIR)/$(NAME).iso
ELFFILE :=  $(OUTDIR)/$(NAME).elf
BINFILE :=  $(OUTDIR)/$(NAME).bin
MAPFILE :=  $(OUTDIR)/$(NAME).map
SYMFILE :=  $(OUTDIR)/$(NAME).sym

CC      :=  $(ARCH)-elf-gcc
AR      :=  $(ARCH)-elf-ar
OBJCOPY :=  $(ARCH)-elf-objcopy

SUBDIRS :=  arch/$(ARCH)
SRCLIST :=  $(foreach d,$(SUBDIRS),$(shell find $(d) -type f -name '*.S' -o -name '*.c'))
OBJLIST :=  $(patsubst %,$(OUTDIR)/%.o,$(SRCLIST))
DEPLIST :=  $(patsubst %.c.o,%.c.d,$(filter %.c.o,$(OBJLIST)))

CFLAGS  :=  -c -std=c11 -Iinclude -DKERNEL -DARCH=$(ARCH)
CFLAGS  +=  -Wall -Wextra -Werror=implicit -Wno-cast-function-type
CFLAGS  +=  -ffreestanding -ffunction-sections -fdata-sections

ifeq ($(DEBUG), 1)
	CFLAGS  +=  -g -DDEBUG
else
	CFLAGS  +=  -g -DNDEBUG
endif

DEPGEN   =  -MT $@ -MMD -MP -MF $(basename $@).t
LFLAGS  :=  -nostdlib -lgcc -T arch/$(ARCH)/linker.ld -Wl,-Map=$(MAPFILE),--gc-sections

CFLAGS  +=  -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow -mno-fma
CFLAGS  +=  -fno-pie
LFLAGS  +=  -z max-page-size=0x1000

build:  $(BINFILE) $(SYMFILE)

clean:
	rm -f $(OBJLIST)
	rm -f $(DEPLIST)
	rm -f $(ELFFILE)
	rm -f $(BINFILE)
	rm -f $(MAPFILE)
	rm -f $(SYMFILE)

$(ELFFILE): $(OBJLIST)
	mkdir -p $(@D)
	$(CC) $(LFLAGS) -o $@ $^

$(BINFILE): $(ELFFILE)
	mkdir -p $(@D)
	$(OBJCOPY) --strip-debug $< $@

$(SYMFILE): $(ELFFILE)
	mkdir -p $(@D)
	$(OBJCOPY) --only-keep-debug $< $@

$(OUTDIR)/%.S.o: %.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(OUTDIR)/%.c.o: %.c $(OUTDIR)/%.c.d
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEPGEN) -o $@ $<
	mv -f $(basename $@).t $(basename $@).d


iso: build
	rm -rf $(ISODIR)
	mkdir -p $(ISODIR)/boot/grub
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	cp $(BINFILE) $(ISODIR)/boot/$(NAME).bin
	grub-mkrescue -o $(ISOFILE) $(ISODIR) 

run: iso
	qemu-system-x86_64 -accel kvm -smp 4 -m 64 \
	    -cdrom $(ISOFILE) -vga vmware -serial stdio -gdb tcp::4444 -boot order=d

$(DEPLIST): ;
.PRECIOUS: $(DEPLIST)

-include $(DEPLIST)
