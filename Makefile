TOPTARGETS := all clean install

# SUBDIRS := $(wildcard */.)
SUBDIRS := efi
$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)