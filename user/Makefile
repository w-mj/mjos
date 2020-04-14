
subdirs := shell

INCLIST := $(SYSROOT)/usr/include/mjos

CC      :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-gcc
CXX     :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-g++
AR      :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-ar
OBJCOPY :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-objcopy
LD      :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-ld
NM      :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-nm

export CC CXX AR OBJCOPY LD NM CFLAGS TEMPLATEFILE ARCH CXXFLAGS TESTINC DISKDIR NAME INCLIST

build: $(subdirs)

test:  $(subdirs)

clean: $(subdirs)

$(subdirs): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

FORCE:
