
subdirs := shell

# TODO: 简化include
INCLIST := sys_header $(CURDIR)/../kernel/include $(CURDIR)/../kernel/arch/$(ARCH)/include \
		   $(CURDIR)/../kernel/libc/include $(CURDIR)/../kernel/libc++/include

CRT0 := $(CURDIR)/crt0.o

export INCLIST CRT0

build: $(subdirs)

test:  $(subdirs)

clean: $(subdirs)
	rm -f crt0.o

$(subdirs): $(CRT0) FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(CRT0): crt0.c
	$(CC) $(CFLAGS) crt0.c -o crt0.o

FORCE:
