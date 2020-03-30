
srclist := process.c \
		   scheduler.c \
		   elf.c \
		   ipc.c \
		   elfpiggy.S \
		   userprocesses.c \
		   pthread.c \
		   process_helper.cpp \
		   signal.cpp
subdirs :=

include $(TEMPLATEFILE)

elfpiggy.S: mkelf
userprocesses.c: mkelf

mkelf: mkuser.cpp
	g++ mkuser.cpp -o mkuser.out && ./mkuser.out

toclean += mkuser.out elfpiggy.S userprocesses.c