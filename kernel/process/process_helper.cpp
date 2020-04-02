#include <delog.h>
#include <process/process.h>
#include <fs/vfs.hpp>
#include <string.hpp>
#include <memory/kmalloc.h>
#include <cpu.h>
#include <asm.h>
#include <memory/page.h>

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
    ProcessDescriptor *process = thiscpu_var(current)->process;
    auto file = root_fs->root->open(process->path);
    int size = file->size;
    if ((size_t)process->linear_end < size) {
        size_t remain = size - (size_t)process->linear_end;
        size_t pages = ROUND_UP(remain, PAGESIZE) / PAGESIZE;
        assert(read_cr3() == process->cr3);
        normal_pages_alloc(process->cr3, 0, pages);
        process->linear_end = (void*)((u64)process->linear_end + PAGESIZE * pages);
    }
    void *linear = (char *)0;
//    while (size > 0) {
//        file->read((char *)linear, 512);
//        linear = (void *)((u64)linear + 512);
//        size -= 512;
//    }
//    void *ss = &shell_start;
    file->read((char *)linear, size); // 进程的线性区起始地址就是0
//    for (int i = 0; i < size; i++) {
//        if (((char *)ss)[i] != ((char *)linear)[i]) {
//            die();
//        }
//    }
    parse_elf64(linear);
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
