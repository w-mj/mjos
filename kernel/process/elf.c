#include <process/elf.h>
#include <delog.h>

#define ELFMAGu32 (ELFMAG0 | (ELFMAG1 << 8) | (ELFMAG2 << 16) | (ELFMAG3 << 24))
void parse_elf64(void *addr) {
	Elf64_Ehdr *head = (Elf64_Ehdr *)addr;
	if (*(u32*)head->e_ident != ELFMAGu32) {
		logi("not a elf header");
		_ss(head->e_ident);
	} else {
		logi("elf64 pass");
	}
}
