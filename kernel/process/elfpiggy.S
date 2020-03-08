#include <linkage.h>
.align 4096
GLOBAL(elf_addr)
	.incbin "elf.o"
GLOBAL(elf_addr_end)
