//
// Created by wmj on 3/25/20.
//

#ifndef OS_PIPE_HPP
#define OS_PIPE_HPP

#include <fs/vfs.hpp>
#include <spin.h>
namespace PIPE {
class PIPE_File: public VFS::File {
private:
    // 固定大小的循环缓冲
    // rpos == wpos为空
    // rpos == wpos + 1为满
    // 读写指针的值皆为应当被读或写的第一个字节位置
    int rpos;  // 读取指针位置
    int wpos;  // 写入指针位置
    int pipe_size;
    char *pipe = nullptr;
    int round(int a);
    Spin spin;  // 读写锁
public:
    explicit PIPE_File(int size=4096);
    ~PIPE_File() override;

    bool empty();
    bool full();
    int cal_size();  // 管道内有效数据长度

    int tell() override;
    int seek(int pos, int whence) override;
    int read(char *, int len) override;
    int write(const char *, int len) override;
    void close() override;

    void stat(kStat *st) override ;
    int getdent(char *, int) override;

};

}

#endif //OS_PIPE_HPP
