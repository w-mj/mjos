
target  := arch.o
subdirs := $(ARCH)

build: $(subdirs)
	ln -f -s $(ARCH)/$(ARCH).o $(target)

clean: $(subdirs)
	rm -f $(target)

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

FORCE:
