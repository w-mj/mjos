#include "ext2_fs.h"
#include "mm/buf.h"
#include "delog/delog.h"
#include "stat.h"
#include "bits.h"

#include <cstring>
#include <iostream>

#include <cmath>
using namespace EXT2;

EXT2_FS::EXT2_FS(Dev::BlockDevice* dev): FS(dev) {
    // mount();
    status = VFS::FS_unmounted;
}

void EXT2_FS::printFS() {
    using namespace std;
    cerr << endl;
    cerr << "文件系统中Inode数量 " << sb->inodes_count << endl;
    cerr << "以块为单位的文件系统大小 " << sb->blocks_count << endl;
    cerr << "文件系统中保留块数目 " << sb->r_blocks_count << endl;
    cerr << "文件系统中空闲块数目 " << sb->free_blocks_count << endl;
    cerr << "文件系统中空闲Inode数量 " << sb->free_inodes_count << endl;
    cerr << "第一个使用的块号 " << sb->first_data_block << endl;
    cerr << "块大小 " << block_size << endl;
    cerr << "逻辑片大小 " << sb->log_frag_size << endl;
    cerr << "每组块数 " << sb->blocks_per_group << endl;
    cerr << "每组碎片数 " << sb->frags_per_group << endl;
    cerr << "每组Inode数 " << sb->inodes_per_group << endl;
    cerr << "上次挂载时间 " << sb->mtime << endl;
    cerr << "上次写入超级块的时间 " << sb->wtime << endl;
    cerr << "未进行检查的挂载次数 " << sb->mnt_count << endl;
    cerr << "必须执行检查前的最大挂载次数 " << sb->max_mnt_count << endl;
    cerr << "Magic " << sb->magic << endl;
    cerr << "文件系统的状态 " << sb->state << endl;

    cerr << endl << "组描述符表：" << endl;
    for (EXT2_GD* gdt: gdt_list) {
        cerr << "块位图块号 " << gdt->get_gd()->block_bitmap << endl;
        cerr << "索引节点位图的块号 " << gdt->get_gd()->inode_bitmap << endl;
        cerr << "第一个索引节点表块的块号 " << gdt->get_gd()->inode_table << endl;
        cerr << "组中空闲块的个数 " << gdt->get_gd()->free_blocks_count << endl;
        cerr << "组中空闲索引节点的个数 " << gdt->get_gd()->free_inodes_count << endl;
        cerr << "组中目录的个数 " << gdt->get_gd()->used_dirs_count << endl;
        cerr << endl;
    }
}


int EXT2_FS::block_to_pos(int block) {
    // _si(block);
    if (block == 0)
        return 1024;
    return (block - 1) * block_size + 1024;
}

int EXT2_FS::inode_to_pos(int inode_n) {
    inode_n--;
    _si(inode_n);
    _si(sb->inodes_per_group);
    int group_n = inode_n / sb->inodes_per_group;
    inode_n = inode_n % sb->inodes_per_group;
    
    int ans = block_to_pos(gdt_list[group_n]->get_gd()->inode_table) + inode_n * sizeof(Inode);
    _sx(ans);
    return ans;

    // inode_n--;
    // int max_inodes_in_group = 8 * block_size;
    // auto it = gdt_list.begin();
    // while (it != gdt_list.end() && inode_n > max_inodes_in_group) {
    //     inode_n -= max_inodes_in_group;
    //     it++;
    // }
    // _error(it == gdt_list.end());
    // _error(inode_n > max_inodes_in_group);

    // return block_to_pos((*it)->get_gd()->inode_table) + inode_n * sizeof(Inode);
}

void EXT2_FS::mount() {
    sb = new SuperBlock();
    // dev->seek(1024);  // 跳过引导块
    MM::Buf sb_buf(1024);
    dev->read(sb_buf, 1024, 1024);  // 先读1k的超级块
    memmove(sb, sb_buf.data, 1024);

    magic = sb->magic;
    _sx(magic);

    // _si(sb->log_block_size);
    // _si(sb->blocks_count);
    // _si(sb->blocks_per_group);

    // 1024 * (1 << sb->log_block_size) * 8;
    block_size = 1024 * (1 << sb->log_block_size);
    // dev->seek(block_size + 1024);  // 指向GDT
    _u32 read_pos = block_size + 1024;
    group_cnt = sb->inodes_count / sb->inodes_per_group;
    for (_u32 i = 0; i < group_cnt; i++) {
        GroupDescriptor* gtp = new GroupDescriptor();
        dev->read(sb_buf, read_pos, sizeof(GroupDescriptor));
        memmove(gtp, sb_buf.data, sizeof(GroupDescriptor));
        gdt_list.push_back(new EXT2_GD(this, gtp, i));
        read_pos += sizeof(GroupDescriptor);
    }

    // 数据块位图
    // dev->seek());
    dev->read(sb_buf, block_to_pos(gdt_list.front()->get_gd()->inode_bitmap), block_size);
    // _sa(sb_buf.data, 1024);

    // 第一个索引节点 / 
    // dev->seek(block_to_pos(gdt_list.front()->inode_table));
    // dev->read(sb_buf, sizeof(Inode));
    // dev->read(sb_buf, sizeof(Inode));
    // Inode* root_inode = new Inode();
    // memmove(root_inode, sb_buf.data, sizeof(Inode));
    // _u32 root_inode_pos = gdt_list.front()->get_gd()->inode_table;
    EXT2_DEntry* ext2_entry = new EXT2_DEntry(this, nullptr, 2, VFS::Directory, "/");
    root = ext2_entry;
    // ext2_entry->inflate();
    // ext2_entry->load_children();
    
    // _sa(sb_buf.data, 1024);
    printFS();
    //printInode(ext2_entry->ext2_inode->i);
    status = VFS::FS_mounted;
}

_u32 EXT2_FS::alloc_inode(EXT2_GD **ret_gd) {
    _u32 inode_per_group = sb->inodes_count / group_cnt;
    int i = 0;
    for (EXT2_GD* gdd: gdt_list) {
        _u32 t = gdd->alloc_inode();
        if (t != 0) {
            _si(t);
            sb->free_inodes_count--;
            if (ret_gd != nullptr)
                *ret_gd = gdd;
            return t + i * inode_per_group;
        }
        i++;
    }
    return 0;
}

_u32 EXT2_FS::alloc_block(EXT2_GD **ret_gd) {
    _u32 block_per_group = sb->blocks_count / group_cnt;
    int i = 0;
    for (EXT2_GD* gdd: gdt_list) {
        _u32 t = gdd->alloc_block();
        if (t != 0) {
            _si(t);
            sb->free_blocks_count--;
            if (ret_gd != nullptr)
                *ret_gd = gdd;
            return t + i * block_per_group;
        }
    }
    return 0;
}

void EXT2_FS::release_inode(_u32 inode_n, EXT2_GD **ret_gd) {
    _u32 inode_per_group = sb->inodes_count / group_cnt;
    _u32 which_group = inode_n / inode_per_group;
    _u32 inode_in_group = inode_n % inode_per_group;
    gdt_list[which_group]->release_inode(inode_in_group);
    sb->free_inodes_count++;
}

void EXT2_FS::release_block(_u32 block_n, EXT2_GD **ret_gd) {
    _u32 block_per_group = sb->blocks_count / group_cnt;
    _u32 which_group = block_n / block_per_group;
    _u32 block_in_group = block_n % block_per_group;
    gdt_list[which_group]->release_block(block_in_group);
    sb->free_blocks_count++;
}

void EXT2_FS::write_super() {
    _u32 super_pos = 1;  // super block 
    MM::Buf buf(sizeof(SuperBlock));
    memcpy(buf.data, sb, sizeof(SuperBlock));
    for (size_t i = 0; i < gdt_list.size(); i++) {
        dev->write(buf, block_to_pos(super_pos), sizeof(SuperBlock));
        super_pos += sb->blocks_per_group;
    }
    _d_end();
}

void EXT2_FS::write_gdt() {
    _u32 super_pos = 2; // 第一个组描述符表位置
    MM::Buf buf(gdt_list.size() * sizeof(GroupDescriptor));
    _u32 s_pos = 0;

    for (EXT2_GD *x: gdt_list) {
        memcpy(buf.data + s_pos, x->get_gd(), sizeof(GroupDescriptor));
        s_pos += sizeof(GroupDescriptor);
        x->write_inode_bitmap();
        x->write_block_bitmap();
    }
    for (size_t i = 0; i < gdt_list.size(); i++) {
        dev->write(buf, block_to_pos(super_pos), sizeof(GroupDescriptor));
        super_pos += sb->blocks_per_group;
    }
    _d_end();
}

EXT2_GD *EXT2_FS::get_inode_group(_u32 inode_n) {
    return gdt_list[inode_n / sb->inodes_per_group];
}

EXT2_GD *EXT2_FS::get_block_group(_u32 block_n) {
    return gdt_list[block_n / sb->blocks_per_group];
}


EXT2_FS::~EXT2_FS() {
    for (auto x: gdt_list) {
        delete x;
    }
    delete sb;
    delete root;
}
