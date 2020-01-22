
SRCLIST := delog.c string.c vsnprintf.c
INCLIST := ../include/

OBJLIST := $(patsubst %, %(OUTDIR)/%.o, $(SRCLIST))
DEPLIST := $(patsubst %.c.o, %.c.d, %(filter %.c.o, %(OBJLIST)))
INCFLAG := $(addprefix -I, $(INCLIST))
CFLAGS  :=  -c -std=c11 $(INCFLAG) -DKERNEL -DARCH=$(ARCH)
CFLAGS  +=  -Wall -Wextra -Werror=implicit -Wno-cast-function-type
CFLAGS  +=  -ffreestanding -ffunction-sections -fdata-sections
ifeq ($(DEBUG), 1)
	CFLAGS  +=  -g -DDEBUG
else
	CFLAGS  +=  -g -DNDEBUG
endif

