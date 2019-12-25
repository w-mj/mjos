TOPTARGETS := all clean install

# SUBDIRS := $(wildcard */.)
SUBDIRS := efi
$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

clean: 
	rm -rf build/

.PHONY: $(TOPTARGETS) $(SUBDIRS)