srclist := init.c biosreg.c bioscall.S
target  := realmode.o
subdirs := 

objlist := $(addsuffix .o, $(srclist))
deplist := $(addsuffix .d, $(srclist))

INCFLAG := $(addprefix -I, $(INCLIST))

DEPGEN   =  -MT $@ -MMD -MP -MF $(basename $@).t
toclean += $(target) $(deplist) $(objlist) pasyms.h realmode.lds

RMCFLAGS := -nostdinc -m16 -g -Os -Wall -Wstrict-prototypes -march=i386 
RMCFLAGS += -mregparm=3 -fno-strict-aliasing -fomit-frame-pointer -fno-pic 
RMCFLAGS += -mno-mmx -mno-sse -ffreestanding -fno-stack-protector 
RMCFLAGS += -Wno-address-of-packed-member -mpreferred-stack-boundary=2

clean: $(subdirs)
	rm -f $(toclean)

build: $(subdirs) $(target)

$(subobjs): $(subdirs)

%.o: % %.d
	$(CC) $(RMCFLAGS) $(INCFLAG) $(DEPGEN) -c -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

realmode.lds: realmode.lds.S pasyms.h
	gcc -E -P $< -o $@

pasyms.h: $(objlist)
	nm $^ | sed -n -r -e 's/^([0-9a-fA-F]+) [ABCDGRSTVW] (.+)$$/pa_\2 = \2;/p' | sort | uniq > $@

RMLDFLAG := -m elf_x86_64  -z max-page-size=0x1000 -m elf_i386 --emit-relocs 
realmode.elf: realmode.lds $(objlist) 
	$(LD) $(RMLDFLAG) -T $< $(objlist) -o $@

realmode.bin: realmode.elf
	$(OBJCOPY) -O binary $^ $@

realmode.o: realmode.bin rmpiggy.S
	$(CC) -c $(CFLAGS) $(INCFLAG) rmpiggy.S -o $@

$(deplist): ;

include $(deplist)

FORCE:

