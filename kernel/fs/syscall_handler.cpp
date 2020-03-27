//
// Created by wmj on 3/25/20.
//
#include <fs/syscall_handler.h>
#include <process/process.h>
#include <cpu.h>
#include <fs/vfs.hpp>

VFS::FS *root_fs = nullptr;

extern "C" int do_open(const char *);
extern "C" int do_read(int , char *, size_t);
extern "C" int do_write(int , const char*, size_t);

int do_open(const char *path) {
    ThreadDescriptor *thread= thiscpu_var(current);
    ProcessDescriptor *process = thread->process;
    int fd = 0;
    // 找到一个空闲的文件描述符
    for (; fd < CFG_PROCESS_FDS; fd++) {
        if (process->fds[fd] == NULL)
            break;
    }
    if (fd == CFG_PROCESS_FDS)
        return -1;
    process->fds[fd] = root_fs->root->open(os::string(path));
    return fd;
}

int do_read(int fd, char *buf, size_t size) {
    ThreadDescriptor *thread= thiscpu_var(current);
    ProcessDescriptor *process = thread->process;
    if (process->fds[fd] == NULL) {
        return -1;
    }
    auto *file = static_cast<VFS::File*>(process->fds[fd]);
    return file->read(buf, size);
}

int do_write(int fd, const char *data, size_t size) {
    ThreadDescriptor *thread= thiscpu_var(current);
    ProcessDescriptor *process = thread->process;
    if (process->fds[fd] == NULL) {
        return -1;
    }
    auto *file = static_cast<VFS::File*>(process->fds[fd]);
    return file->write(data, size);
}