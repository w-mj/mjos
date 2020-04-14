#include <delog.h>
#include <process/process.h>
#include <fs/vfs.hpp>
#include <string.hpp>
#include <memory/kmalloc.h>
#include <cpu.h>
#include <asm.h>
#include <memory/page.h>
#include <process/elf.h>

extern "C" void copy_file(ProcessDescriptor *to, ProcessDescriptor *from) {
    for (int i = 0; i < CFG_PROCESS_FDS; i++) {
        if (from->fds[i] != NULL) {
            to->fds[i] = from->fds[i];
            static_cast<VFS::File*>(to->fds[i])->open();
        }
    }
}

static inline char *read_file(VFS::File *file, size_t offset, size_t len) {
    char *ret = (char *)kmalloc_s(len);
    file->seek(offset, VFS::SET);
    file->read(ret, len);
    return ret;
}

//extern "C" void *shell_start;
extern "C" int do_real_exec() {
    ThreadDescriptor  *thread = thiscpu_var(current);
    ProcessDescriptor *process = thread->process;
    auto file = root_fs->root->open(process->path);
    auto elf = (Elf64_Ehdr*)kmalloc_s(sizeof(Elf64_Ehdr));
    file->read((char *)elf, sizeof(Elf64_Ehdr));  // 读入elf头
    assert(*(u32*)elf->e_ident == 0x464c457f);
    // 处理程序表
    size_t pt_size = elf->e_phnum * elf->e_phentsize;   // pt表大小
    auto pt = (Elf64_Phdr*) kmalloc_s(pt_size);
    file->seek(elf->e_phoff, VFS::SET);
    file->read((char *)pt, pt_size);  // 读入pt表
    // 遍历程序头表
    for (int i = 0; i < elf->e_phnum; i++) {
		auto *segment = pt + i;
		if (segment->p_type != PT_LOAD)
		    continue;
		u64 mem_end = segment->p_vaddr + segment->p_memsz;   // 计算内存结束地址
		if (mem_end > (u64)process->linear_end) {
		    // 分配更多页面
		    size_t remain = mem_end - (u64)process->linear_end;
		    size_t pages = ROUND_UP(remain, PAGESIZE) / PAGESIZE;
		    normal_pages_alloc(process->cr3, (u64)process->linear_end, pages);
            process->linear_end = (void*)((u64)process->linear_end + PAGESIZE * pages);
		}
		file->seek(segment->p_offset, VFS::SET);
		file->read((char *)segment->p_vaddr, segment->p_filesz);  // 读入文件
		char *ss = (char *)segment->p_vaddr;
		if (segment->p_filesz < segment->p_memsz) {
		    // 文件内容大小小于内存空间大小，多出来的空间置零
		    memset((void *)(segment->p_vaddr + segment->p_filesz), 0, segment->p_memsz - segment->p_filesz);
		}
	}
    // 处理段表
    size_t st_size = elf->e_shnum * elf->e_shentsize;
    auto * st = (Elf64_Shdr *)kmalloc_s(st_size);
    file->seek(elf->e_shoff, VFS::SET);
    file->read((char *)st, st_size);

    Elf64_Shdr *shstr_sec = st + elf->e_shstrndx;
    size_t shstr_size = shstr_sec->sh_size;
    char *shstr = read_file(file, shstr_sec->sh_offset, shstr_size);
    Elf64_Shdr *symtab_head = elf_get_section(st, st_size, shstr, ".symtab");
    Elf64_Shdr *strtab_head = elf_get_section(st, st_size, shstr, ".strtab");
    Elf64_Sym *symtab = (Elf64_Sym*)read_file(file, symtab_head->sh_offset, symtab_head->sh_size);
    char *strtab = read_file(file, strtab_head->sh_offset, strtab_head->sh_size);
    Elf64_Sym *on_signal = elf_get_symbol(symtab, symtab_head->sh_size, strtab, "on_signal");

    process->signalHandler = reinterpret_cast<SignalHandler>(on_signal->st_value);

    kfree_s(symtab_head->sh_size, symtab);
    kfree_s(strtab_head->sh_size, strtab);
    kfree_s(shstr_size, shstr);
    kfree_s(pt_size, pt);
    kfree_s(st_size, st);
    file->close();

    // 设置堆地址
    process->heap_start = process->linear_end;
    process->heap_end   = process->linear_end;

//    auto *signal_symbol = elf_get_symbol(elf, "on_signal");
//    process->signalHandler = reinterpret_cast<SignalHandler>(signal_symbol->st_value);
    u64 ip = elf->e_entry;
    user_return((void*)ip, (void*)((u64)thread->stack));
    die();
    return 0;  // make compiler happy.
}

extern "C" int do_exec(const char *path) {
    ProcessDescriptor *process = thiscpu_var(current)->process;
    size_t size;
    if (process->path != NULL) {
        size = strlen(process->path) + 1;
        kfree_s(size, process->path);
    }
    size = strlen(path) + 1;
    process->path = (char *)kmalloc_s(size);
    memcpy(process->path, path, size);
    return do_real_exec();
}
