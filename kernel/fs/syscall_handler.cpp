//
// Created by wmj on 3/25/20.
//
#include <fs/syscall_handler.h>
#include <process/process.h>
#include <cpu.h>
#include <fs/vfs.hpp>
#include <stdio.h>

VFS::FS *root_fs = nullptr;

extern "C" int do_open(const char *);
extern "C" int do_read(int , char *, size_t);
extern "C" int do_write(int , const char*, size_t);
extern "C" int do_close(int);
extern "C" int do_get_attr(int, FILE*);

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

VFS::File *get_file(int fd) {
    ThreadDescriptor *thread= thiscpu_var(current);
    ProcessDescriptor *process = thread->process;
    if (process->fds[fd] == NULL) {
        return nullptr;
    }
    return static_cast<VFS::File*>(process->fds[fd]);
}

int do_read(int fd, char *buf, size_t size) {
    auto file = get_file(fd);
    if (file == nullptr)
        return -1;
    return file->read(buf, size);
}

int do_write(int fd, const char *data, size_t size) {
    auto file = get_file(fd);
    if (file == nullptr)
        return -1;
    return file->write(data, size);
}

int do_close(int fd) {
    auto file = get_file(fd);
    if (file == nullptr)
        return -1;
    file->close();
    return 0;
}

int do_get_attr(int fd, FILE *file) {
    auto f = get_file(fd);
    if (f == nullptr)
        return -1;
    file->fno = fd;
    file->size = f->size;
    return 0;
}