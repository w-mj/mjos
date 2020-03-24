#pragma once
#include "ext2_fs.h"

namespace EXT2 {
    class EXT2_GD {
        _u8 *inode_bitmap = nullptr, *block_bitmap = nullptr;
        GroupDescriptor* gd;
        EXT2_FS *fs;
        _u32 group_n;
    public:
        EXT2_GD(EXT2_FS *fs, GroupDescriptor* gd, _u32 n);
        ~EXT2_GD();
        GroupDescriptor *get_gd();
        _u8 *get_inode_bitmap();
        _u8 *get_block_bitmap();
        _u32 alloc_inode();
        _u32 alloc_block();
        void release_inode(_u32 inode_n);
        void release_block(_u32 block_n);
        void write_inode_bitmap();
        void write_block_bitmap();
    };
}