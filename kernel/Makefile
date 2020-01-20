
ELFFILE :=  $(OUTDIR)/kernel.elf
BINFILE :=  $(OUTDIR)/kernel.bin
MAPFILE :=  $(OUTDIR)/kernel.map
SYMFILE :=  $(OUTDIR)/kernel.sym


SUBDIRS :=  arch/$(ARCH) misc
ifneq ($(wildcard arch/$(ARCH)/bit32),)
$(info "detected 32bits code")
	SRCLIST32 := $(shell find arch/$(ARCH)/bit32 $(d) -type f -name '*.S' -o -name '*.c')
endif
ifneq ($(wildcard arch/$(ARCH)/bit16),)	
$(info "detected 16bits code")
	SRCLIST16 := $(shell find arch/$(ARCH)/bit16 $(d) -type f -name '*.S' -o -name '*.c')
endif
SRCLIST64 := $(foreach d,$(SUBDIRS),$(shell find $(d) -type f -name '*.S' -o -name '*.c'))
SRCLIST64 := $(filter-out $(SRCLIST32), $(SRCLIST64))
SRCLIST64 := $(filter-out $(SRCLIST16), $(SRCLIST64))
OBJLIST64 := $(patsubst %,$(OUTDIR)/%.o,$(SRCLIST64))
OBJLIST32 := $(patsubst %,$(OUTDIR)/%.o,$(SRCLIST32)) 
OBJLIST16 := $(patsubst %,$(OUTDIR)/%.o,$(SRCLIST16)) 
OBJLIST   := $(OBJLIST16) $(OBJLIST32) $(OBJLIST64)
DEPLIST   := $(patsubst %.c.o,%.c.d,$(filter %.c.o,$(OBJLIST)))

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

include arch/$(ARCH)/Makefile.config

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
