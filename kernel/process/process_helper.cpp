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

//extern "C" void *shell_start;
extern "C" int do_real_exec() {
    ThreadDescriptor  *thread = thiscpu_var(current);
    ProcessDescriptor *process = thread->process;
    auto file = root_fs->root->open(process->path);
    int size = file->size;
    u64 linear_size = (u64)process->linear_end - (u64)process->linear_start;
    if (linear_size < size) {
        size_t remain = size - linear_size;
        size_t pages = ROUND_UP(remain, PAGESIZE) / PAGESIZE;
        assert(read_cr3() == process->cr3);
        normal_pages_alloc(process->cr3, (u64)process->linear_end, pages);
        process->linear_end = (void*)((u64)process->linear_end + PAGESIZE * pages);
    }
    file->read((char *)process->linear_start, size); // 进程的线性区起始地址就是0
    file->close();
//   parse_elf64(process->linear_start);
    auto *elf = (Elf64_Ehdr *)process->linear_start;

    auto *signal_symbol = elf_get_symbol(elf, "on_signal");

    process->signalHandler = reinterpret_cast<SignalHandler>(signal_symbol->st_value);

    u64 ip = elf->e_entry;
    user_return((void*)ip, (void*)((u64)thread->stack));
    while(1);
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
