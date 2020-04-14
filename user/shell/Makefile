
srclist := shell.cpp keycode2ascii.cpp
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
	$(CXX) $(objlist) -O0 -o shell.run
	yes | cp -f shell.run $(DISKDIR)

%.cpp.o: %.cpp %.cpp.d
	$(CXX) $(INCFLAG) -c $(DEPGEN) -O0 -o $@ $<
	@ mv -f $(basename $@).t $(basename $@).d

$(deplist): ;

include $(deplist)