#pragma once
#include "vfs.h"
#include "ext2_fs.h"
#include "ext2_dentry.h"
#include "ext2_inode.h"
#include "../../arch/x86_64/include/types.h"

namespace EXT2 {
    class EXT2_FS;
    class EXT2_DEntry;
    class EXT2_Inode;

    class EXT2_File: public VFS::File {
        EXT2_DEntry *ext2_dentry;
        EXT2_Inode *ext2_inode;
        EXT2_FS *ext2_fs;
    public:
        EXT2_File(EXT2_DEntry *);
        EXT2_File(EXT2_DEntry *, EXT2_Inode *);
        int tell();
        int seek(int pos, int whence);
        int read(char*, int size);
        int write(const char*, int size);
        void resize(_u32 new_size);
    };
}