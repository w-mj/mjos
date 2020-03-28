
subdirs := misc arch memory process dev syscall libc libc++ fs libcxxrt
srclist := spin.c init.cpp
binfile := kernel.bin
elffile := kernel.elf
mapfile := kernel.map
symfile := kernel.sym

crti_src := arch/$(ARCH)/crti.S
crtn_src := arch/$(ARCH)/crtn.S
crti_obj := crti.o
crtn_obj := crtn.o
#crtbegin_obj := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
#crtend_obj   := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
crtbegin_obj := crtbegin.o
crtend_obj := crtend.o
uwind_obj := unwind-c.o  unwind-dw2-fde.o  unwind-dw2.o  unwind-sjlj.o


objs    := $(addsuffix .o, $(srclist))
deps    := $(addsuffix .d, $(srclist))
subobjs := $(foreach d, $(subdirs), $(addsuffix .obj, $(d)/$(d)))
# objlist := $(crti_obj) $(crtbegin_obj) $(subobjs) $(objs) $(unwind_obj) $(crtend_obj) $(crtn_obj)
objlist := $(subobjs) $(objs)

DEPGEN   =  -MT $@ -MMD -MP -MF $(basename $@).t
INCLIST := $(CURDIR)/include $(CURDIR)/arch/$(ARCH)/include \
			$(CURDIR)/libc/include $(CURDIR)/libc++/include
LFLAGS  := -nostdlib -lgcc -T arch/$(ARCH)/linker.ld -Wl,-Map=$(mapfile)
INCFLAG := $(addprefix -I, $(INCLIST))

include arch/$(ARCH)/Makefile.config

export INCLIST CFLAGS INCFILE CPPFLAGS

build: $(subdirs) $(binfile) $(symfile)

test: $(subdirs)
all: build
debugbackground: build
debug: build

clean: $(subdirs)
	rm -f $(binfile)
	rm -f $(elffile)
	rm -f $(mapfile)
	rm -f $(symfile)
	rm -f $(objs)
	rm -f $(deps)
	rm -f $(crti_obj) $(crtn_obj)
	@rm -f compile*.d

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(subobjs): $(subdirs)

$(crti_obj): $(crti_src)
	$(CC) $(CFLAGS) $< -o $@

$(crtn_obj): $(crtn_src)
	$(CC) $(CFLAGS) $< -o $@

%.c.o: %.c %.c.d
	$(CC) $(CFLAGS) $(INCFLAG) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

%.S.o: %.S %.S.d
	$(CC) $(CFLAGS) $(INCFLAG) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

%.cpp.o: %.cpp %.cpp.d
	$(CXX) $(CXXFLAGS) $(INCFLAG) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

$(binfile): $(elffile)
	$(OBJCOPY) --strip-debug $< $@

$(elffile): $(objlist)
	$(CC) $(LFLAGS) -o $@ $^

$(symfile): $(elffile)
	$(OBJCOPY) --only-keep-debug $< $@

$(deps): ;

FORCE:
