//
// Created by wmj on 3/25/20.
//
#include <fs/syscall_handler.h>
#include <process/process.h>
#include <cpu.h>
#include <fs/vfs.hpp>
#include <stdio.h>
#include <cwalk.h>

VFS::FS *root_fs = nullptr;

extern "C" int do_open(const char *);
extern "C" int do_read(int , char *, size_t);
extern "C" int do_write(int , const char*, size_t);
extern "C" int do_close(int);
extern "C" int do_get_attr(int, FILE*);
extern "C" int do_fstat(int fno, kStat *st);
extern "C" int do_link(const char *, const char *);
extern "C" int do_unlink(const char *);
extern "C" int do_lseek(int, int, int);

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

int do_fstat(int fd, kStat *st) {
    auto f = get_file(fd);
    f->stat(st);
    if (fd == 0 || fd == 1 || fd == 2) {
        st->mode |= S_IFCHR;
    }
    return 0;
}

int do_link(const char *old_path, const char *new_path) {
    auto *old = root_fs->root->get_path(old_path);
    size_t dir_len = strlen(new_path);
    if (new_path[dir_len - 1] != '/') {
        cwk_path_get_dirname(new_path, &dir_len);
        auto *new_dir = root_fs->root->get_path(os::string(new_path, dir_len).c_str());
        if (new_dir == nullptr)
            return -1;
        old->link(new_dir, new_path + dir_len);
    } else {
        auto *new_dir = root_fs->root->get_path(new_path);
        if (new_dir == nullptr)
            return -1;
        old->link(new_dir);
    }
    return 0;
}

int do_unlink(const char *path) {
    auto *file = root_fs->root->get_child(path);
    if (file == nullptr)
        return -1;
    file->unlink();
}

int do_lseek(int fd, int ptr, int dir) {
    auto *file = get_file(fd);
    file->seek(ptr, dir);
    return 0;
}