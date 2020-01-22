
subdirs := $(ARCH)

build: $(subdirs)
	ln -f -s $(ARCH)/$(ARCH).o arch.o

clean: $(subdirs)

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

FORCE:
