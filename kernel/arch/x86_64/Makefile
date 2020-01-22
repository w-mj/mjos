
srclist := boot.S copy.S kernel.c
target  := x86_64.o
subdirs := driver

objlist := $(addsuffix .o, $(srclist))
deplist := $(addsuffix .d, $(srclist))

subobjs := $(foreach d, $(subdirs), $(addsuffix .o, $(d)/$(d)))
INCFLAG := $(addprefix -I, $(INCLIST))
DEPGEN   =  -MT $@ -MMD -MP -MF $(basename $@).t

clean: $(subdirs)
	rm -f $(target)
	rm -f $(objlist)
	rm -f $(deplist)

build: $(subdirs) $(target)

$(target): $(objlist) $(subobjs)
	$(LD) -relocatable $^ -o $@

$(subobjs): $(subdirs)

%.o: % %.d
	$(CC) $(CFLAGS) $(INCFLAG) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(deplist): ;

include $(deplist)

FORCE:



