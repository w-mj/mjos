#pragma once

#include "types.h"
#include "fs/vfs.h"
#include "dev/block_dev.hpp"
#include "ext2_disk_stru.h"
#include "ext2_inode.h"
#include "ext2_dentry.h"
#include "ext2_group_descriptor.h"

#include <list.hpp>
namespace EXT2 {

    class EXT2_Inode;
    class EXT2_DEntry;
    class EXT2_GD;

    class EXT2_FS: public VFS::FS {

        void printFS();
        void printInode(Inode*);
    public:
        int block_to_pos(int block);
        int inode_to_pos(int inode_n);

        SuperBlock* sb;
        std::list<EXT2_GD*> gdt_list;

        _u32 block_size;
        _u32 group_cnt;

        _u32 alloc_inode(EXT2_GD **ret_gd=nullptr);
        _u32 alloc_block(EXT2_GD **ret_gd=nullptr);
        void release_inode(_u32 inode_n, EXT2_GD **ret_gd=nullptr);
        void release_block(_u32 block_n, EXT2_GD **ret_gd=nullptr);

        void write_super();
        void write_gdt();

        EXT2_GD *get_inode_group(_u32 inode_n);
        EXT2_GD *get_block_group(_u32 inode_n);

        EXT2_FS(Dev::BlockDevice* dev);
        void mount();
        ~EXT2_FS();
    };

}
