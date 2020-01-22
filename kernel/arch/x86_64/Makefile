OUTDIR := .
ELFFILE :=  $(OUTDIR)/kernel.elf
BINFILE :=  $(OUTDIR)/kernel.bin
MAPFILE :=  $(OUTDIR)/kernel.map
SYMFILE :=  $(OUTDIR)/kernel.sym

RMDIR := arch/$(ARCH)/realmode
RMLDS := $(RMDIR)/realmode.lds
RMELFFILE  :=  $(RMDIR)/realmode.elf
RMBINFILE  :=  $(RMDIR)/realmode.bin

SUBDIRS :=  arch/$(ARCH) misc
ifneq ($(wildcard arch/$(ARCH)/ia32),)
$(info "detected 32 bits code")
	SRCLIST32 := $(shell find arch/$(ARCH)/ia32 $(d) -type f -name '*.S' -o -name '*.c')
endif
ifneq ($(wildcard $(RMDIR)),)	
$(info "detected 16 bits code")
	SRCLIST16 := $(shell find $(RMDIR) $(d) -type f -name '*.S' -o -name '*.c')
	SRCLIST16 := $(filter-out arch/x86_64/realmode/realmode.lds.S, $(SRCLIST16))
endif
SRCLIST := $(foreach d,$(SUBDIRS),$(shell find $(d) -type f -name '*.S' -o -name '*.c'))
SRCLIST64 := $(filter-out $(SRCLIST32), $(SRCLIST))
SRCLIST64 := $(filter-out $(SRCLIST16), $(SRCLIST64))
OBJLIST16 := $(patsubst %,$(OUTDIR)/%.o,$(SRCLIST16))
OBJLIST32 := $(patsubst %,$(OUTDIR)/%.o,$(SRCLIST32))
OBJLIST64 := $(patsubst %,$(OUTDIR)/%.o,$(SRCLIST64))
OBJLIST   := $(OBJLIST16) $(OBJLIST32) $(OBJLIST64)
DEPLIST   := $(patsubst %.c.o,%.c.d,$(filter %.c.o,$(OBJLIST)))

INCLIST := include arch/$(ARCH)/include
INCFLAG := $(addprefix -I, $(INCLIST))

CFLAGS  :=  -c -std=c11 $(INCFLAG) -DKERNEL -DARCH=$(ARCH)
CFLAGS  +=  -Wall -Wextra -Werror=implicit 
CFLAGS  +=  -ffreestanding -ffunction-sections -fdata-sections

ifeq ($(DEBUG), 1)
	CFLAGS  +=  -g -DDEBUG
else
	CFLAGS  +=  -g -DNDEBUG
endif

DEPGEN   =  -MT $@ -MMD -MP -MF $(basename $@).t
LFLAGS  :=  -nostdlib -lgcc -T arch/$(ARCH)/linker.ld -Wl,-Map=$(MAPFILE),--gc-sections

include arch/$(ARCH)/Makefile.config
$(foreach n, $(SRCLIST16), $(eval $(n).o_D := $(CFLAGS16)))
$(foreach n, $(SRCLIST32), $(eval $(n).o_D := $(CFLAGS32)))
$(foreach n, $(SRCLIST64), $(eval $(n).o_D := $(CFLAGS64)))
build:  $(BINFILE) $(SYMFILE)

clean:
	rm -f $(OBJLIST)
	rm -f $(DEPLIST)
	rm -f $(ELFFILE)
	rm -f $(BINFILE)
	rm -f $(MAPFILE)
	rm -f $(SYMFILE)
	rm -f $(RMLDS)
	rm -f $(RMELFFILE)
	rm -f $(RMBINFILE)

$(RMELFFILE): $(OBJLIST16)
	$(NM) $(OBJLIST16) | sed -n -r -e 's/^([0-9a-fA-F]+) [ABCDGRSTVW] (.+)$/pa_\2 = \2;/p' | sort | uniq > $(RMDIR)/pasyms.h
	$(CC) -E $(RMLDS).S -o $(RMLDS)
	$(LD) $(LFLAGS16) -o $@ $^

$(RMBINFILE): $(RMELFFILE)
	$(OBJCOPY) -O $< $@

arch/x86_64/rmpiggy.S: $(RMBINFILE)

$(ELFFILE): $(OBJLIST)
	@ mkdir -p $(@D)
	$(CC) $(LFLAGS) $(LFLAGS64) -o $@ $^

$(BINFILE): $(ELFFILE)
	@ mkdir -p $(@D)
	$(OBJCOPY) --strip-debug $< $@

$(SYMFILE): $(ELFFILE)
	@ mkdir -p $(@D)
	$(OBJCOPY) --only-keep-debug $< $@

$(OUTDIR)/%.S.o: %.S
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) $($*.S.o_D) -o $@ $<

$(OUTDIR)/%.c.o: %.c $(OUTDIR)/%.c.d
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) $($*.c.o_D) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d


$(DEPLIST): ;
.PRECIOUS: $(DEPLIST)

-include $(DEPLIST)
