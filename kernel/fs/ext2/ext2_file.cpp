#undef DEBUG
#include "fs/ext2/ext2_file.hpp"
#include <string.h>
#include <delog.h>
#include <algorithm.hpp>
#include <errno.h>
using namespace EXT2;


EXT2_File::EXT2_File(EXT2_DEntry *d, EXT2_Inode *i) {
    ext2_dentry = d;
    ext2_inode = i;
    ext2_fs = i->ext2_fs;
    type = d->type;
    pos = 0;
    size = i->size;
//    logd("打开文件");
    // _si(size);
}

EXT2_File::EXT2_File(EXT2_DEntry *d) {
    if (d->ext2_inode == nullptr)
        d->inflate();
    ext2_dentry = d;
    ext2_inode = d->ext2_inode;
    ext2_fs = d->ext2_inode->ext2_fs;
    type = d->type;
    pos = 0;
    size = d->ext2_inode->size;
//    logd("打开文件");
    // _si(size);
}


int EXT2_File::tell() {
    return pos;
}

int EXT2_File::seek(int pos, int whence) {
    switch (whence) {
        case VFS::SEEK::CUR:
            this->pos += pos;
            break;
        case VFS::SEEK::END:
            this->pos = os::min(this->size + pos, this->size);
            break;
        case VFS::SEEK::SET:
            this->pos = pos;
            break;
        default:
            loge("Unknow whence %d", whence);
    }
    // _si(this->pos);
    return this->pos;
}

int EXT2_File::read(char *buf, int len) {
    len = os::min(len, size - pos);
    if (len == 0)
        return 0;
    logd("read start %d, size %d, size %d", pos, len, size);
    _u32 read_len = 0, this_read_len;
    _u32 block_size = ext2_fs->block_size;  // 块大小
    _u32 byte_in_block = pos % block_size;  // 第一个字节在块中的位置
    _u32 nth_block = pos / block_size;  // 第一个字节在第n个块，pos以0开始计数，不需要加1
    auto it = ext2_inode->iter_at(nth_block);
    _u32 block_n = *it;  // 第一个字节的块号
//    _si(block_n);
    _u32 pos_in_fs = ext2_fs->block_to_pos(block_n);  // 第一个块在文件系统中的位置

    read_len = os::min((_u32)len, block_size - byte_in_block);
    ext2_fs->dev->read(buf, pos_in_fs + byte_in_block, read_len);  //  读入数据
    // _sa(buff.data, 1024);
    while (read_len < (_u32)len && pos + read_len < (_u32)size) {
        // 后续的数据都是从块起始位置开始
        it++;  // 指向下一个块
        block_n = *it;  // 下一个块号
        _si(block_n);
        pos_in_fs = ext2_fs->block_to_pos(block_n);  // 下一个块的位置
//        _si(pos_in_fs);
        this_read_len = ext2_fs->dev->read(buf + read_len, pos_in_fs, os::min(block_size, len - read_len));  // 读入
        read_len += this_read_len;
    }
    pos += read_len;
    return read_len;
}

int EXT2_File::write(const char *buf, int len) {
    // _si(pos);
    
    resize(len + pos);
    // ext2_inode->print();
    _u32 write_len = 0, this_write_len;
    _u32 block_size = ext2_fs->block_size;  // 块大小
    _u32 byte_in_block = pos % block_size;  // 第一个字节在块中的位置
    _u32 nth_block = pos / block_size;  // 第一个字节在第n个块，pos以0开始计数，不需要加1
    auto it = ext2_inode->iter_at(nth_block);
    _u32 block_n = *it;  // 第一个字节的块号
    _u32 pos_in_fs = ext2_fs->block_to_pos(block_n);  // 第一个块在文件系统中的位置
    //_si(pos + len);
    this_write_len = os::min(block_size - byte_in_block, (_u32)len);
    ext2_fs->dev->write(buf, pos_in_fs + byte_in_block, this_write_len);  // 向第一个块写数据
    write_len = this_write_len;
    while (write_len < (_u32)len) {
        it++;  // 指向下一个块
        block_n = *it;  // 下一个块号，其实这里可以自动分配空间，不用resize
        pos_in_fs = ext2_fs->block_to_pos(block_n);  // 下一个块的位置
        this_write_len = os::min(block_size, len - write_len);
        ext2_fs->dev->write(buf + write_len, pos_in_fs, this_write_len);  // 写
        write_len += this_write_len;
    }
    pos += write_len;
    //_si(pos);
    if (pos > size)
        size = pos;
    return write_len;
}

void EXT2_File::resize(_u32 new_size) {
    EXT2_FS *fs = ext2_fs;
    _u32 current_block = ext2_inode->blocks;
    _u32 target_block = new_size / fs->block_size + (new_size % fs->block_size > 0);
    logd("resize from %d to %d", current_block, target_block);
    if (target_block == current_block) {
        logd("do not alloc size form %d to %d", size, new_size);
        size = new_size;
        ext2_inode->size = new_size;
        ext2_inode->write_inode();
        return;
    }
    auto it = ext2_inode->alloc_iter(); 
    if (target_block > current_block) {
        // 分配空间
        while (target_block != current_block) {
            *it;  // 利用解引用自动分配空间
            it++;
            current_block++;
        }
    } else {
        // 缩小空间
        while (target_block != current_block) {
            it--;
            _u32 to_release = *it;
            fs->release_block(to_release);
            current_block--;
        }
    }
    size = new_size;
    ext2_inode->size = size;
    ext2_inode->blocks = target_block;
    ext2_inode->write_inode();
    ext2_fs->write_gdt();
    ext2_fs->write_super();
}

void EXT2_File::stat(kStat *st) {
    st->dev = 1234; //ext2_fs->dev
    st->ino = ext2_inode->ino;
    st->mode = ext2_inode->mode;
    st->nlink = ext2_inode->nlink;
    st->uid = ext2_inode->uid;
    st->gid = ext2_inode->gid;
    st->rdev = 4321;
    st->size = size;
    st->blksize = 5678; //ext2_inode->blkbits;
    st->blocks = ext2_inode->blocks;
    st->atime = ext2_inode->atime;
    st->mtime = ext2_inode->mtime;
    st->ctime = ext2_inode->ctime;
}

int EXT2_File::getdent(char *buff, int count) {
    if ((type & S_IFDIR) == 0)
        return -1;
    EXT2::DirEntry temp{};
    int t = read((char *)&temp, (int)(u64)(&((EXT2::DirEntry*)nullptr)->name));
    if (t == 0)
        return 0;
    if (count < temp.name_len + 1) {
        seek(-(int)(u64)(&((EXT2::DirEntry*)nullptr)->name), VFS::SEEK::CUR);
        return -1;
    }
    read(buff, temp.name_len);
    buff[temp.name_len] = 0;
    return 1;
}
