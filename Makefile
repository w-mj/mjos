TOPTARGETS := all clean install build

# SUBDIRS := $(wildcard */.)
SUBDIRS := boot
$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

clean: 
	rm -rf build/

.PHONY: $(TOPTARGETS) $(SUBDIRS)
