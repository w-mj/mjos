
target  := arch.obj
subdirs := $(ARCH)

build: $(subdirs)
	ln -f -s $(ARCH)/$(ARCH).obj $(target)

clean: $(subdirs)
	rm -f $(target)

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

FORCE:
