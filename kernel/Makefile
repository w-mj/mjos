
subdirs := misc
binfile := kernel.bin
elffile := kernel.elf
mapfile := kernel.map
symfile := kernel.sym

subobjs := $(foreach d, $(subdirs), $(addsuffix .o, $(d)/$(d)))
objlist := $(subobjs)

INCLIST := $(CURDIR)/include $(CURDIR)/arch/$(ARCH)/include
LFLAGS  := -nostdlib -lgcc -T arch/$(ARCH)/linker.ld -Wl,-Map=$(mapfile),--gc-sections
-include arch/$(ARCH)/Makefile.config
export 

build: $(subdirs) $(binfile) $(symfile)

clean: $(subdirs)
	rm -f $(binfile)
	rm -f $(elffile)
	rm -f $(mapfile)
	rm -f $(symfile)

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(binfile): $(elffile)
	$(OBJCOPY) --strip-debug $< $@

$(elffile): $(subobjs)
	$(CC) $(LFLAGS) -o $@ $^

$(symfile): $(elffile)
	$(OBJCOPY) --only-keep-debug $< $@

FORCE:
