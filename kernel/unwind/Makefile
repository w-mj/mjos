srclist := unwind-c.c unwind-dw2.c unwind-dw2-fde-dip.c unwind-sjlj.c
target  := unwind.obj

deplist := $(addsuffix .d, $(srclist))
objlist := $(addsuffix .o, $(srclist))

FLAGS := -g -O2 -DIN_GCC -W -Wall -Wno-narrowing -Wwrite-strings -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
		 -Wold-style-definition -fpic -mlong-double-80 -DUSE_ELF_SYMVER  -g -DIN_LIBGCC2 \
		 -fbuilding-libgcc -fno-stack-protector  -fpic -mlong-double-80 -DUSE_ELF_SYMVER  -DENABLE_DECIMAL_BID_FORMAT\
		 -DHAVE_CC_TLS  -DUSE_TLS -fexceptions -fvisibility=hidden -DHIDE_EXPORTS

DEPGEN   =  -MT $@ -MMD -MP -MF $(basename $@).t

INCFLAG := $(addprefix -I, $(INCLIST))

build: $(target)

clean:
	rm -rf $(objlist) $(deplist)

$(target): $(objlist)
	$(LD) -relocatable $^ -o $@

%.o: %
	$(CC) $(CFLAGS) $(INCFLAG) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

