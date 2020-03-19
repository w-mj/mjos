#pragma once
#include "ext2_fs.h"
#include "ext2_inode.h"

namespace EXT2
{
    class EXT2_FS;
    class EXT2_Inode;
    class EXT2_File;

    class EXT2_DEntry: public VFS::DEntry {
    private:
        EXT2_FS* ext2_fs;

        void write_children();
        VFS::File *get_file();
    protected:
    public:
        EXT2_Inode* ext2_inode = nullptr;

        _u8 sync = 0;  // 为1时需要调用inflate从磁盘读取

        // 父文件夹可以与此项目有不同的文件系统
        EXT2_DEntry(EXT2_FS*, VFS::DEntry*, _u32, _u8, const std::string&);
        void inflate();
        void load_children();

        VFS::DEntry* mkdir(const std::string& name);
        VFS::DEntry* create(const std::string& name);
        void unlink(VFS::DEntry *d);  // 删除子项目
        void unlink();  // 删除自己
        void unlink_children();
        // 在tar中创建自己的硬链接
        VFS::DEntry *link(DEntry *tar, const std::string& s="");
        // 将本目录中的item移动到另一个目录中
        VFS::DEntry *move(DEntry *dir, const std::string& new_name="");
        bool empty();

        VFS::DEntry *copy(DEntry *dir, const std::string& new_name);
        VFS::DEntry *symlink(DEntry *file, const std::string& new_name="");

        VFS::DEntry *follow();

        ~EXT2_DEntry();
    };
} // namespace EXT2
