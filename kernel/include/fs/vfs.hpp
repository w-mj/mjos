#pragma once

#include <types.h>
#include <dev/block_dev.hpp>
#include <list.hpp>
#include <string.hpp>
#include <sys/kstat.h>

namespace VFS
{

    enum FileType {
        Unknown=0,
        RegularFile,
        Directory,
        CharacterDevice,
        BlockDevice,
        NamedPipe,
        Socket,
        SymbolLink
    };

    enum SEEK {
        SET=0,
        CUR=1,
        END=2
    };

    class Inode;
    class FS;
    class File;
    class DEntry;

    /**
     * NAMEi
     */
    class NameI {
    public:
        os::string name;
        NameI *next=nullptr, *prev=nullptr;

        NameI(const os::string name, NameI *p=nullptr);

        static NameI *from_str(const os::string &);
        ~NameI();
    };

    /**
     * 一个目录项
     */
    class DEntry {
    private:
    protected:
        virtual File* get_file()=0;
    public:
        FS* fs;
        _u32 inode_n;  // inode编号，懒加载
        Inode* inode = nullptr;
        DEntry* parent;
        _u8 type;

        os::string name;

        os::list<DEntry*> children;

        virtual void inflate()=0;
        virtual void load_children()=0;

        // 在当前目录项创建文件夹
        virtual VFS::DEntry* mkdir(const os::string& new_name)=0;
        // 在当前目录下创建空文件
        virtual VFS::DEntry* create(const os::string& new_name)=0;
        // 在当前目录下删除子项目，修改自己与子文件夹的引用关系
        virtual void unlink(DEntry*)=0;
        // 删除自己，释放空间，会由ulink(DEntry*)调用
        virtual void unlink()=0;  
        // 删除一个目录下的所有项目
        virtual void unlink_children()=0;
        // 判断一个目录是否为空
        virtual bool empty()=0; 
        // 在另一个目录项中创建自己的硬链接
        virtual VFS::DEntry *link(DEntry *dir, const os::string& s="")=0;
        // 把自己移动到另一个目录中
        virtual VFS::DEntry *move(DEntry *dir, const os::string& new_name="")=0;
        // 打开文件
        File *open();
        // 把自己复制到另一个文件夹中，新文件夹中文件的Entry
        virtual VFS::DEntry *copy(DEntry *dir, const os::string& new_name="")=0;
        // 在当前文件夹下创建另一个文件file的符号链接
        virtual VFS::DEntry *symlink(DEntry *file, const os::string& new_name="")=0;
        // 找到一个符号链接指向的文件
        virtual VFS::DEntry *follow()=0;

        File *open(const os::string& name);
        DEntry* get_child(const os::string& name);
        DEntry* get_child(const NameI *namei, DEntry **path=nullptr);
        DEntry *get_path(const NameI *namei, os::string* fname=nullptr);
        DEntry *get_path(const char *path);
        os::string printed_path();


        virtual ~DEntry() {}
    };

    enum {
        FS_unmounted = 0,
        FS_mounted
    };

    class FS {
    public:
        Dev::BlockDevice* dev=nullptr;  // 设备
        // _u32 flag;  // 安装标志
        _u32 magic;  // 文件系统标志

        _u8 status; // 状态， 0为未挂载，1为已挂载
        
        DEntry* root = nullptr;  // 根目录
        _i32 count;  // 引用计数器

        void** xattr;  // 指向扩展属性的指针
        FS(Dev::BlockDevice* dev): dev(dev) {}

        virtual void mount() = 0;
    };
    
    class Inode {
    public:
        _u32 ino;  // 索引节点号
        _i32 counter;  // 引用计数器
        _u16 mode; // 文件类型与访问权限
        _u32 nlink;  // 硬链接数目
        _u32 uid;  // 拥有者
        _u32 gid;  // 组
        Dev::BlockDevice* dev;  // 设备
        _u64 size;  // 文件字节
        _u32 atime;  // 访问时间
        _u32 mtime;  // 写文件时间
        _u32 ctime;  // 修改索引节点时间
        _u32 blkbits;  // 块位数
        _u32 version;  // 版本号（每次使用后递增）
        _u32 blocks;  // 文件的块数
        _u16 bytes;  // 文件最后一个块的字节数
        _u8 sock;  // 如果文件是一个socket则为0
        int lock;  // 自旋锁

        FS* fs=nullptr;
    public:
        Inode(FS* fs1):dev(fs1->dev), fs(fs1) {}
        // 将Inode标记为脏，找时机写回设备        
        virtual void dirty()=0;
        virtual ~Inode() {}
    };


    /**
     * 打开的文件
     */
    class File {
    protected:
        int open_cnt = 0;
    public:
        int pos;
        int size;
        int type;
        int open_mode;
        void open();
        virtual int tell()=0;
        virtual int seek(int pos, int whence)=0;
        virtual int read(char*, int size)=0;
        virtual int write(const char*, int size)=0;
        virtual void close();
        virtual void stat(kStat *st)=0;
        virtual ~File() {}

    };

    _u8 mode_to_type(_u8 mode);
    
}; // namespace VFS

extern VFS::FS *root_fs;
extern VFS::File *stdin;
extern VFS::File *stdout;
extern VFS::File *stderr;
