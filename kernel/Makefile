
ELFFILE :=  $(OUTDIR)/kernel.elf
BINFILE :=  $(OUTDIR)/kernel.bin
MAPFILE :=  $(OUTDIR)/kernel.map
SYMFILE :=  $(OUTDIR)/kernel.sym


SUBDIRS :=  arch/$(ARCH)
SRCLIST :=  $(foreach d,$(SUBDIRS),$(shell find $(d) -type f -name '*.S' -o -name '*.c'))
OBJLIST :=  $(patsubst %,$(OUTDIR)/%.o,$(SRCLIST))
DEPLIST :=  $(patsubst %.c.o,%.c.d,$(filter %.c.o,$(OBJLIST)))

INCLIST := include arch/$(ARCH)/include
INCFLAG := $(addprefix -I, $(INCLIST))

CFLAGS  :=  -c -std=c11 $(INCFLAG) -DKERNEL -DARCH=$(ARCH)
CFLAGS  +=  -Wall -Wextra -Werror=implicit -Wno-cast-function-type
CFLAGS  +=  -ffreestanding -ffunction-sections -fdata-sections

ifeq ($(DEBUG), 1)
	CFLAGS  +=  -g -DDEBUG
else
	CFLAGS  +=  -g -DNDEBUG
endif

DEPGEN   =  -MT $@ -MMD -MP -MF $(basename $@).t
LFLAGS  :=  -nostdlib -lgcc -T arch/$(ARCH)/linker.ld -Wl,-Map=$(MAPFILE),--gc-sections

# 以下三行为x86_64专用
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
	@ mkdir -p $(@D)
	$(CC) $(LFLAGS) -o $@ $^

$(BINFILE): $(ELFFILE)
	@ mkdir -p $(@D)
	$(OBJCOPY) --strip-debug $< $@

$(SYMFILE): $(ELFFILE)
	@ mkdir -p $(@D)
	$(OBJCOPY) --only-keep-debug $< $@

$(OUTDIR)/%.S.o: %.S
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(OUTDIR)/%.c.o: %.c $(OUTDIR)/%.c.d
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d


$(DEPLIST): ;
.PRECIOUS: $(DEPLIST)

-include $(DEPLIST)
