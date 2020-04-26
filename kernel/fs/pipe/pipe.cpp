//
// Created by wmj on 3/25/20.
//

#include <fs/pipe/pipe.hpp>
#include <memory/kmalloc.h>
#include <algorithm.hpp>
#include <errno.h>

using namespace PIPE;

PIPE_File::PIPE_File(int size): pipe_size(size) {
    pipe = static_cast<char *>(kmalloc_s(size));
    wpos = 0;
    rpos = 0;
    this->size = size;
};

int PIPE_File::tell() {
    return size - cal_size();
}

int PIPE_File::seek(int pos, int whence) {
    return 0;
}

PIPE_File::~PIPE_File() {
    close();
//    kfree_s(pipe_size, pipe);
//    pipe = nullptr;
}

int PIPE_File::read(char *buf, int len) {
    if (empty() || len == 0)
        return 0;
    raw_spin_take(&spin);
    // 读取的数据长度
    int read_size = os::min(len, pipe_size - rpos, cal_size());
    memcpy(buf, pipe + rpos, read_size);
    rpos = round(rpos + read_size);
    raw_spin_give(&spin);
    return read(buf + read_size, len - read_size) + read_size;
}

int PIPE_File::round(int a) {
    return (a + pipe_size) % pipe_size;
}

bool PIPE_File::empty() {
    return rpos == wpos;
}

bool PIPE_File::full() {
    return round(wpos + 1) == rpos;
}

int PIPE_File::cal_size() {
    return round(wpos - rpos);
}

int PIPE_File::write(const char *data, int len) {
    if (full() || len == 0)
        return 0;
    raw_spin_take(&spin);
    int write_size = os::min(len, pipe_size - wpos, pipe_size - cal_size() - 1);
    memcpy(pipe + wpos, data, write_size);
    wpos = round(wpos + write_size);
    raw_spin_give(&spin);
    return write(data + write_size, len - write_size) + write_size;
}

void PIPE_File::close() {
    open_cnt--;
    if (open_cnt == 0) {
        kfree_s(pipe_size, pipe);
        pipe = nullptr;
    }
}

void PIPE_File::stat(kStat *st) {
    // TODO: 之后再填入文件属性
    st->dev = 1;
    st->ino = 4;
    // type=fifo, privilege=777
    st->mode = S_IFIFO | S_IRWXU | S_IRWXG| S_IRWXO;
    st->nlink = open_cnt;
    st->uid = 1;
    st->gid = 1;
    st->rdev = 1;
    st->size = cal_size();
    st->blksize = 1;
    st->blocks = 1;
    st->atime = 100;
    st->mtime = 110;
    st->ctime = 111;
}

int PIPE_File::getdent(char *, int) {
    return -1;
}
