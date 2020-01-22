
srclist := serial.c console.c 
target  := driver.o 

objlist := $(addsuffix .o, $(srclist))
deplist := $(addsuffix .d, $(srclist))

clean:
	rm -f $(objlist)
	rm -f $(deplist)
	rm -f $(target)

build: $(target)

INCFLAG := $(addprefix -I, $(INCLIST))
DEPGEN   =  -MT $@ -MMD -MP -MF $(basename $@).t
$(target): $(objlist)
	$(LD) -relocatable $(objlist) -o $(target)

%.o: % %.d
	$(CC) $(CFLAGS) $(INCFLAG) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

$(deplist): ;

include $(deplist)
