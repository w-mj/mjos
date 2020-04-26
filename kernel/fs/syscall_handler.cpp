//
// Created by wmj on 3/25/20.
//
#include <fs/syscall_handler.h>
#include <process/process.h>
#include <process/scheduler.h>
#include <cpu.h>
#include <fs/vfs.hpp>
#include <stdio.h>
#include <cwalk.h>
#include <delog.h>
#include <algorithm.hpp>

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
extern "C" int do_getdent(int, char*, int);
extern "C" int do_chdir(const char*);
extern "C" int do_getcwd(char *, int);

VFS::File *get_file(int fd) {
    ThreadDescriptor *thread= thiscpu_var(current);
    ProcessDescriptor *process = thread->process;
    if (process->fds[fd] == NULL) {
        return nullptr;
    }
    return static_cast<VFS::File*>(process->fds[fd]);
}

static inline VFS::DEntry *get_cwd() {
    auto *process = thiscpu_var(current)->process;
    auto *ret = static_cast<VFS::DEntry*>(process->cwd);
    return ret;
}

static inline void set_cwd(VFS::DEntry *cwd) {
    thiscpu_var(current)->process->cwd = cwd;
}

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
    auto *p = get_cwd()->get_path(path);
    if (p == nullptr)
        return -1;
    process->fds[fd] = p->open();
    if (process->fds[fd] == nullptr)
        return -1;
    return fd;
}

int do_read(int fd, char *buf, size_t size) {
    auto file = get_file(fd);
    if (file == nullptr)
        return -1;
    if (file->tell() == file->size) {
        // 挂自己
        // thread_wait(ThreadWaitFile, (u64)file);
    }
    return file->read(buf, size);
}

int do_write(int fd, const char *data, size_t size) {
    auto file = get_file(fd);
    if (file == nullptr)
        return -1;
    auto ret = file->write(data, size);
    // thread_resume(ThreadWaitFile, (u64)file);
    return ret;
}

int do_close(int fd) {
    auto file = get_file(fd);
    if (file == nullptr)
        return -1;
    file->close();
    thiscpu_var(current)->process->fds[fd] = NULL;
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
    auto *old = get_cwd()->get_path(old_path);
    size_t dir_len = strlen(new_path);
    if (new_path[dir_len - 1] != '/') {
        cwk_path_get_dirname(new_path, &dir_len);
        auto *new_dir = get_cwd()->get_path(os::string(new_path, dir_len).c_str());
        if (new_dir == nullptr)
            return -1;
        old->link(new_dir, new_path + dir_len);
    } else {
        auto *new_dir = get_cwd()->get_path(new_path);
        if (new_dir == nullptr)
            return -1;
        old->link(new_dir);
    }
    return 0;
}

int do_unlink(const char *path) {
    auto *file = get_cwd()->get_child(path);
    if (file == nullptr)
        return -1;
    file->unlink();
    return 0;
}

int do_lseek(int fd, int ptr, int dir) {
    return get_file(fd)->seek(ptr, dir);
}

int do_getdent(int fd, char *buff, int count) {
    return get_file(fd)->getdent(buff, count);
}

int do_chdir(const char *path) {
    auto *dir = get_cwd()->get_path(path);
    if (dir == nullptr)
        return -1;
    set_cwd(dir);
    return 1;
}

int do_getcwd(char *buf, int size) {
    auto *cwd = get_cwd();
    os::vector<VFS::DEntry *> st;
    do {
        st.push_back(cwd);
        cwd = cwd->parent;
    } while (cwd != cwd->parent);
    
    while (!st.empty()) {
        cwd = st.pop_back();
        if (size == 0)
            return -1;
        *buf++ = '/';
        size--;
        if (size < cwd->name.size())
            return -1;
        if (cwd->name[0] != '/') {
            strcpy(buf, cwd->name.c_str());
            buf += cwd->name.size();
            size -= cwd->name.size();
        }
    }
    *buf = 0;
    return 0;
}

int do_stat(const char *path, kStat *st) {
    auto *file = get_cwd()->get_path(path);
    if (file == nullptr)
        return -1;
    if (st != nullptr) {
        auto *opened = file->open();
        opened->close();
    }
    return 0;
}
