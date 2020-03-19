#pragma once
#include "fs/vfs.h"
#include "types.h"
#include <cstdio>

namespace RealFile {

class RealFile: public VFS::File {
    FILE *fp = nullptr;
    char *fname = nullptr;
    char *buf;
    int pos;
public:
    RealFile(const char* name);
    ~RealFile();
    int tell();
    int seek(int pos, int whence);
    int read(char*, int size);
    int write(const char*, int size);
    void open(const char* mode);
    void close();
};

}
