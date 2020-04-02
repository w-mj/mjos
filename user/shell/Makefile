
srclist := shell.cpp
deplist := $(addsuffix .d, $(srclist))
objlist := $(addsuffix .o, $(srclist))

DEPGEN   = -MT $@ -MMD -MP -MF $(basename $@).t
INCFLAG := $(addprefix -I, $(INCLIST))

target := shell.run

build: $(target)

clean:
	rm -rf $(deplist) $(objlist) $(target)

test:

$(target): $(objlist)
	ld $(CRT0) $(objlist) -o shell.run
	yes | cp -f shell.run $(DISKDIR)

shell.cpp.o: shell.cpp shell.cpp.d
	g++ $(CXXFLAGS) $(INCFLAG) $(DEPGEN) -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

$(deplist): ;

include $(deplist)