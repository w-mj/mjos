#include "real_file.h"
#include "delog/delog.h"
#include <cstring>
#include <sys/stat.h>

using namespace RealFile;

RealFile::RealFile::RealFile(const char* name) {
    int ss = strlen(name);
    fname = new char[ss];
    strcpy(fname, name);

    struct stat statbuf;
    stat(fname, &statbuf);
    size=statbuf.st_size;
    _si(statbuf.st_nlink);

    this->open("rb");

    _dbg_log("open file %s size %d", name, size);

    buf = new char[size];
    fread(buf, size, 1, fp);
    fclose(fp);
    fp = nullptr;
}

RealFile::RealFile::~RealFile() {
    delete fname;
}

int RealFile::RealFile::tell() {
    if (fp != nullptr)
        return pos;
    return -1;
}

int RealFile::RealFile::seek(int pos, int whence) {
    switch (whence) {
        case SEEK_SET:
            return (this->pos = pos);
        case SEEK_CUR:
            return (this->pos = this->pos + pos);
        case SEEK_END:
            return (this->pos = size + pos);
    }
    return -1;
}

int RealFile::RealFile::read(char *p, int size) {
    memmove(p, buf + pos, size);
    return size;
}

int RealFile::RealFile::write(const char *p, int size) {
    memmove(buf + pos, p, size);
    return size;
}

void RealFile::RealFile::open(const char* mode) {
    _pos();
    _ss(fname);
    _ss(mode);
    fp = fopen(fname, mode);
    if (fp == nullptr) {
        perror(__func__);
        exit(1);
    }
    pos = 0;
}

void RealFile::RealFile::close() {
    _dbg_log("save file");
    open("wb");
    fwrite(buf, size, 1, fp);
    fclose(fp);
    fp = nullptr;
    // VFS::File::close();
}