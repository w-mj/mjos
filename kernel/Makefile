
subdirs := misc arch memory process driver syscall
srclist := spin.c init.c
binfile := kernel.bin
elffile := kernel.elf
mapfile := kernel.map
symfile := kernel.sym

objs    := $(addsuffix .o, $(srclist))
deps    := $(addsuffix .d, $(srclist))
subobjs := $(foreach d, $(subdirs), $(addsuffix .obj, $(d)/$(d)))
objlist := $(subobjs) $(objs)

DEPGEN   =  -MT $@ -MMD -MP -MF $(basename $@).t
INCLIST := $(CURDIR)/include $(CURDIR)/arch/$(ARCH)/include
LFLAGS  := -nostdlib -lgcc -T arch/$(ARCH)/linker.ld -Wl,-Map=$(mapfile),--gc-sections
INCFLAG := $(addprefix -I, $(INCLIST))

include arch/$(ARCH)/Makefile.config

export INCLIST CFLAGS INCFILE CPPFLAGS

build: $(subdirs) $(binfile) $(symfile)

clean: $(subdirs)
	rm -f $(binfile)
	rm -f $(elffile)
	rm -f $(mapfile)
	rm -f $(symfile)
	rm -f $(objs)
	rm -f $(deps)

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(subobjs): $(subdirs)


%.o: % %.d
	$(CC) $(CFLAGS) $(INCFLAG) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

$(binfile): $(elffile)
	$(OBJCOPY) --strip-debug $< $@

$(elffile): $(objlist)
	$(CC) $(LFLAGS) -o $@ $^

$(symfile): $(elffile)
	$(OBJCOPY) --only-keep-debug $< $@

$(deps): ;

FORCE:
