
subdirs := shell

INCLIST := sys_header $(CURDIR)/../kernel/include $(CURDIR)/../kernel/arch/$(ARCH)/include \
		   $(CURDIR)/../kernel/libc/include $(CURDIR)/../kernel/libc++/include

INCFLAG := $(addprefix -I, $(INCLIST))

CRT0 := $(CURDIR)/crt0.o

CC      :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-gcc
CXX     :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-g++
AR      :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-ar
OBJCOPY :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-objcopy
LD      :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-ld
NM      :=  $(TOOLCHAIN_BASE)$(ARCH)-$(NAME)-nm

export CC CXX AR OBJCOPY LD NM CFLAGS TEMPLATEFILE ARCH CXXFLAGS TESTINC DISKDIR NAME

build: $(subdirs)

test:  $(subdirs)

clean: $(subdirs)
	rm -f crt0.o

$(subdirs): $(CRT0) FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(CRT0): crt0.c
	$(CC) $(CFLAGS) $(INCFLAG) crt0.c -o crt0.o

FORCE:
