
subdirs := $(ARCH)

build: $(subdirs)
	ln -f -s $(ARCH)/$(ARCH).obj $(target)

clean: $(subdirs)
	rm -f $(target)

test: $(subdirs)

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

FORCE:
