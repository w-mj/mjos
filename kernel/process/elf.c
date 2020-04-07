#include <process/elf.h>
#include <delog.h>
#include <stdio.h>
#include <string.h>

#define CHK(a, b, ok, fail) do {\
	if ((a) == (b)) {\
		if (ok[0] != '\0') {\
			logi(" %s", ok); \
		} \
	} else {\
		if (fail[0] != '\0') {\
			logw(" %s", fail); \
		} \
		return; \
	} }while(0)

#define CASE(v, str) \
	case v: if (str[0] != '\0') logi(" %s", str); break

#define ELFMAGu32 (ELFMAG0 | (ELFMAG1 << 8)| (ELFMAG2 << 16) | (ELFMAG3 << 24))
void parse_elf64(void *addr) {
	Elf64_Ehdr *head = (Elf64_Ehdr *)addr;
	logi("parse elf64 file");
	CHK(*(u32*)head->e_ident, ELFMAGu32, "", "not a elf headerl");
	CHK(head->e_ident[EI_CLASS], ELFCLASS64, "", "class is not 64");
	CHK(head->e_ident[EI_DATA], ELFDATA2LSB, "", "not little end");
	CHK(head->e_ident[EI_VERSION], EV_CURRENT, "", "elf version err");
	switch(head->e_type) {
		CASE(ET_NONE, "type NONE");
		CASE(ET_REL,  "type rel");
		CASE(ET_EXEC, "type exec");
		CASE(ET_DYN,  "type dynamic");
		CASE(ET_CORE,  "type core");
		CASE(ET_LOPROC,  "type loproc");
		CASE(ET_HIPROC,  "type hiproc");
	}
	CHK(head->e_machine, EM_X86_64, "", "machine is not x86_64");
	CHK(head->e_version, EV_CURRENT, "", "elf version err 2");
	logi(" elf flags %x", head->e_flags);
	logi(" elf head size %d", head->e_ehsize);
	logi(" elf entry %llx", head->e_entry);
	logi(" program header table offset %llx, size %d, num %d", 
			head->e_phoff, head->e_phentsize, head->e_phnum);
	logi(" section header table offset %llx, size %d, num %d",
			head->e_shoff, head->e_shentsize, head->e_shnum);
	logi(" string table index %d", head->e_shstrndx);
	logi(" parse elf program table");
	logi(" offset, virtual, physics, file size, mem size,   type, flags");
	u8 *pt = (u8*)head + head->e_phoff;
	char type[16] = {0};
	char flags[8] = "       ";  // 7 spaces
	while (pt < (u8*)head + head->e_phoff + head->e_phnum * head->e_phentsize) {
		Elf64_Phdr *segment = (Elf64_Phdr *)pt;
		pt += head->e_phentsize;
		switch (segment->p_type) {
			case PT_NULL:         snprintf(type, 16, "NULL");     break;
			case PT_LOAD:         snprintf(type, 16, "load");     break;
			case PT_DYNAMIC:      snprintf(type, 16, "dynamec");  break;
			case PT_INTERP:       snprintf(type, 16, "interp");   break;
			case PT_NOTE:         snprintf(type, 16, "note");     break;
			case PT_SHLIB:        snprintf(type, 16, "shlib");    break;
			case PT_PHDR:         snprintf(type, 16, "phdr");     break;
			case PT_TLS:          snprintf(type, 16, "tls");      break;
			case PT_LOOS:         snprintf(type, 16, "loos");     break;
			case PT_HIOS:         snprintf(type, 16, "hios");     break;
			case PT_LOPROC:       snprintf(type, 16, "loproc");   break;
			case PT_HIPROC:       snprintf(type, 16, "hiproc");   break;
			case PT_GNU_EH_FRAME: snprintf(type, 16, "GNUFrame"); break;
			case PT_GNU_STACK:    snprintf(type, 16, "GNUStack"); break;
		}
		*(u64*)flags = 0;
		if (segment->p_flags & PF_R) flags[0] = 'R';
		if (segment->p_flags & PF_W) flags[1] = 'W';
		if (segment->p_flags & PF_X) flags[2] = 'X';
		flags[3] = 0;

		logi(" %7llx, %8llx, %8llx, %10d, %9d, %6s, %s",
			segment->p_offset, segment->p_vaddr, segment->p_paddr, 
			segment->p_filesz, segment->p_memsz, type, flags);
	}
	logi(" parse elf section table");
	u8 *ht = (u8*)head + head->e_shoff;
	int i = 0;
	Elf64_Shdr *symtbl_sec = NULL;
	Elf64_Shdr *strtbl_sec = NULL;
	logi("    name, addr, offset, size, link, info, entsize, type, flags");
	while (ht < (u8*)head + head->e_shoff + head->e_shentsize * head->e_shnum) {
		Elf64_Shdr *sec = (Elf64_Shdr *)ht;
		ht = ht + head->e_shentsize;
		switch (sec->sh_type) {
			case SHT_NULL:     snprintf(type, 16, "NULL");     break;
			case SHT_PROGBITS: snprintf(type, 16, "PROGBITS"); break;
			case SHT_SYMTAB:   snprintf(type, 16, "symtabl");
			    symtbl_sec = sec;
			    break;
			case SHT_STRTAB:   snprintf(type, 16, "strtab");   break;
			case SHT_RELA:     snprintf(type, 16, "rela");     break;
			case SHT_HASH:     snprintf(type, 16, "hash");     break;
			case SHT_DYNAMIC:  snprintf(type, 16, "dyname");   break;
			case SHT_NOTE:     snprintf(type, 16, "note");     break;
			case SHT_NOBITS:   snprintf(type, 16, "nobits");   break;
			case SHT_REL:      snprintf(type, 16, "rel");      break;
			case SHT_SHLIB:    snprintf(type, 16, "shlib");    break;
			case SHT_DYNSYM:   snprintf(type, 16, "dynsym");   break;
			case SHT_NUM:      snprintf(type, 16, "num");      break;
			case SHT_LOPROC:   snprintf(type, 16, "loproc");   break;
			case SHT_HIPROC:   snprintf(type, 16, "hiproc");   break;
			case SHT_LOUSER:   snprintf(type, 16, "louser");   break;
			case SHT_HIUSER:   snprintf(type, 16, "hiuser");   break;
		}
		*(u64*)flags = 0;
		if (sec->sh_flags & SHF_WRITE)          flags[0] = 'W';
		if (sec->sh_flags & SHF_ALLOC)          flags[1] = 'A';
		if (sec->sh_flags & SHF_EXECINSTR)      flags[2] = 'E';
		if (sec->sh_flags & SHF_RELA_LIVEPATCH) flags[3] = 'R';
		if (sec->sh_flags & SHF_RO_AFTER_INIT)  flags[4] = 'O';
		if (sec->sh_flags & SHF_MASKPROC)       flags[5] = 'M';
		char *name = "noname";
		if (head->e_shstrndx != SHN_UNDEF) {
			u8 *strtab = (u8*)head + head->e_shoff + head->e_shstrndx * head->e_shentsize;
			name = (char *)head + ((Elf64_Shdr *)strtab)->sh_offset + sec->sh_name;
		}
		logi(" %02d %-10s, %5llx, %7llx, %5d, %5d, %5d, %8d, %s, %s",
				i++, name, sec->sh_addr, sec->sh_offset, sec->sh_size, 
				sec->sh_link, sec->sh_info, sec->sh_entsize, type, flags);
		if (strcmp(name, ".strtab") == 0) {
            strtbl_sec = sec;
		}
	}

    if (symtbl_sec == NULL || strtbl_sec == NULL) {
        return;
    }

	Elf64_Sym *sym = (Elf64_Sym *) (symtbl_sec->sh_offset + addr);
	while ((u8*)sym < symtbl_sec->sh_offset + (u8*)head + symtbl_sec->sh_size) {
        char *name = addr + strtbl_sec->sh_offset +sym->st_name;
	    logi("  %s", name);
	    sym = (Elf64_Sym *) ((u8 *) sym + sizeof(Elf64_Sym));
	}
}

Elf64_Shdr *elf_get_section(Elf64_Shdr *st, size_t st_size, char *shstrtab, const char *name) {
    Elf64_Shdr *end = st + st_size;
    while (st != end) {
        if (strcmp(&shstrtab[st->sh_name], name) == 0) {
            return st;
        }
        st++;
    }
    return NULL;
}

Elf64_Sym *elf_get_symbol(Elf64_Sym *symtab, size_t symtab_size, char *strtab, const char *name) {
   Elf64_Sym *end = symtab + symtab_size;
   while (symtab != end) {
       if (strcmp(&strtab[symtab->st_name], name) == 0) {
           return symtab;
       }
       symtab++;
   }
   return NULL;
}