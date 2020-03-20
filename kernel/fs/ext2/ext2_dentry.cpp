#include "ext2_dentry.h"
#include "ext2_file.h"
#include "fs/stat.h"
#include <delog.h>
#include <string.h>
#include <memory/buf.hpp>
#include <time.h>

using namespace EXT2;
EXT2_DEntry::EXT2_DEntry(EXT2_FS* fs1, VFS::DEntry* parent1, 
            _u32 inode_n1, _u8 type1, const std::string& name1) {
    fs = fs1;
    ext2_fs = fs1;
    parent = parent1;
    if (parent == nullptr)
        parent = this;
    inode = nullptr;
    ext2_inode = nullptr;
    inode_n = inode_n1;
    type=type1;
    name = std::string(name1);
    sync = 1;
}
#include <iostream.hpp>
void EXT2_DEntry::load_children() {
    assert(type == VFS::Directory);

    if (ext2_inode != nullptr && children.size() != 0)
        return ;
    inflate();
    _log_info("load children");

    assert(ext2_inode != nullptr);
    //_si(ext2_inode->size);

    Dev::BlockDevice* dev = ext2_fs->dev;
    MM::Buf buf(ext2_fs->block_size);

    DirEntry *temp_str = new DirEntry();
    for (auto x: children)
        delete x;
    children.clear();
    // for (_u32 i = 0; i < ext2_inode->i->blocks; i++) {
    _sp(*ext2_inode);
    ext2_inode->print();
    _u32 all_length = 0;
    for (int i: *ext2_inode) {
        _u32 data_block_pos = ext2_fs->block_to_pos(i);
        dev->read(buf, data_block_pos, ext2_fs->block_size);
        // _sa(buf.data, ext2_fs->block_size);
        _u32 s_pos = 0;
        // _u32 next_length = 12;
        while (all_length < ext2_inode->size) {
            // _sa(buf.data + s_pos, 20);
            memmove(temp_str, buf.data + s_pos, sizeof(DirEntry));
            if (temp_str->rec_len == 0)
                break;
            // if (temp_str->inode == parent->inode_n) {
            //     children.push_back(parent); 
            // } else if (temp_str->inode == inode_n) {
            //     children.push_back(this);
            // } else {
                EXT2_DEntry *sub = new EXT2_DEntry(ext2_fs, this, 
                        temp_str->inode, temp_str->file_type, 
                        std::string((char*)temp_str->name, temp_str->name_len));
                // if (temp_str->inode == parent->inode_n) {
                //     sub->inode = parent->inode; 
                //     sub->sync = 0;
                // } else if (temp_str->inode == inode_n) {
                //     sub->inode = inode;
                //     sub->sync = 0;
                // }
                _si(temp_str->rec_len);
                children.push_back(sub);
            //}
            // std::cout << sub->name << " " << sub->inode_n << std::endl;
            s_pos += temp_str->rec_len;
            all_length += temp_str->rec_len;
        }
    }
}

void EXT2_DEntry::inflate() {
    if (inode != nullptr && sync != 1)
        return;
    _log_info("inflate");
    delete inode;
    EXT2::Inode* disk_inode = new Inode();
    MM::Buf buf(sizeof(Inode));
    _u32 inode_pos = ext2_fs->inode_to_pos(inode_n);
    _si(inode_pos);
    fs->dev->read(buf, inode_pos, sizeof(Inode));
    memcpy(disk_inode, buf.data, sizeof(Inode));
    ext2_inode = new EXT2_Inode(ext2_fs, inode_n, disk_inode);
    ext2_inode->print();
    inode = ext2_inode;
    // type = VFS::mode_to_type(inode->mode);
    sync = 0;
}

static inline int cal_name_len(int s) {
    if (s % 4 == 0)
        return s;
    return 4 - (s % 4) + s;
}

void EXT2_DEntry::write_children() {
    assert(type == VFS::Directory);
    DirEntry new_disk_entry[1];
    EXT2_File parent_body(this, this->ext2_inode);
    _u32 all_length = 0;
    _u32 sub_item_cnt = 0;
    for (VFS::DEntry* de: children) {
        memset(new_disk_entry->name, 0, sizeof(new_disk_entry->name));
        new_disk_entry->file_type = de->type;
        new_disk_entry->inode = de->inode_n;
        strcpy((char*)new_disk_entry->name, de->name.c_str());
        new_disk_entry->name_len = strlen(de->name.c_str());
        if (sub_item_cnt == children.size() - 1) {
            // 最后一个项目充满整个块
            new_disk_entry->rec_len = ext2_fs->block_size - all_length;
        } else {
            new_disk_entry->rec_len = 8 + cal_name_len(new_disk_entry->name_len);
        }
        sub_item_cnt++;
        all_length += new_disk_entry->rec_len;
        parent_body.write((char *)new_disk_entry, new_disk_entry->rec_len);
        // new_disk_entry->name = de->name.c_str();
    }
    // _d_end();
}

VFS::DEntry* EXT2_DEntry::mkdir(const std::string& new_name) {
    assert(type == VFS::Directory);
    EXT2_GD *new_dir_gd = nullptr;
    _u32 new_i_n = ext2_fs->alloc_inode(&new_dir_gd);
    new_dir_gd->get_gd()->used_dirs_count++;
    _u32 new_b_n = ext2_fs->alloc_block();
    // logd("分配节点");
    // MM::Buf buf(1024);
    // ext2_fs->dev->read(buf, ext2_fs->inode_to_pos(new_i_n - 1), 1024);
    // _sa(buf.data, 1024);
    // exit(0);
    load_children();

    // 构建新的Inode
    EXT2::Inode *new_disk_i = new EXT2::Inode();
    // _sp(*new_disk_i);
    new_disk_i->mode = ext2_inode->mode;
    new_disk_i->uid = ext2_inode->uid;
    new_disk_i->size = ext2_fs->block_size;
    new_disk_i->atime = time(0);
    new_disk_i->mtime = time(0);
    new_disk_i->ctime = time(0);
    new_disk_i->gid = ext2_inode->gid;
    new_disk_i->links_count = 2;  // . 和父目录中的自己
    new_disk_i->blocks = 2;
    new_disk_i->flags = 0;
    new_disk_i->block[0] = new_b_n;
    EXT2_Inode *new_i = new EXT2_Inode(ext2_fs, new_i_n, new_disk_i);
    new_i->write_inode();  // 新Inode写入磁盘
    EXT2_DEntry *new_entry = new EXT2_DEntry(ext2_fs, this, new_i_n, VFS::Directory, new_name);
    new_entry->ext2_inode = new_i;
    // 加入到当前目录中
    children.push_back(new_entry);
    // 子目录的DirEntry写入父目录的文件体中
    write_children();
    // EXT2_File parent_body11(this, this->ext2_inode);

    // 修改父目录的Inode
    ext2_inode->nlink++; // 硬链接计数器加1
    ext2_inode->write_inode();

    DirEntry new_disk_entry[1];    
    // 在子目录的文件体中写入.和..
    new_disk_entry->file_type = VFS::Directory;
    EXT2_File child_body(new_entry, new_i);
    new_disk_entry->inode = new_i_n;
    new_disk_entry->rec_len = 12;
    new_disk_entry->name_len = 1;
    new_disk_entry->name[0] = '.';
    new_disk_entry->name[1] = '\0';
    child_body.write((char*)new_disk_entry, new_disk_entry->rec_len);
    new_disk_entry->inode = this->inode_n;
    new_disk_entry->name_len = 2;
    new_disk_entry->rec_len = ext2_fs->block_size - 12;
    new_disk_entry->name[1] = '.';
    new_disk_entry->name[2] = '\0';
    child_body.write((char*)new_disk_entry, new_disk_entry->rec_len);
    // EXT2_File child_bod11y(new_entry, new_i);

    // ext2_fs->sb->
    ext2_fs->write_gdt();
    ext2_fs->write_super();
    return new_entry;
}

VFS::DEntry *EXT2_DEntry::create(const std::string& name) {
    assert(type == VFS::Directory);
    _pos();
    inflate();
    _u32 new_i_n = ext2_fs->alloc_inode();
    assert(new_i_n != 0);
    EXT2::Inode *new_disk_i = new EXT2::Inode();
    // _sp(*new_disk_i);
    new_disk_i->mode = S_IFREG | S_IRUSR | S_IWUSR  | S_IROTH | S_IRGRP;
    new_disk_i->uid = ext2_inode->uid;
    new_disk_i->size = 0;
    new_disk_i->atime = time(0);
    new_disk_i->mtime = time(0);
    new_disk_i->ctime = time(0);
    new_disk_i->gid = ext2_inode->gid;
    new_disk_i->links_count = 1;  // 父目录中的自己
    new_disk_i->blocks = 0;
    new_disk_i->flags = 0;
    EXT2_Inode *new_i = new EXT2_Inode(ext2_fs, new_i_n, new_disk_i);
    new_i->write_inode();  // 新Inode写入磁盘
    EXT2_DEntry *new_entry = new EXT2_DEntry(ext2_fs, this, new_i_n, VFS::RegularFile, name);
    new_entry->ext2_inode = new_i;
    new_entry->inode = new_i;
    load_children();
    children.push_back(new_entry);
    write_children();

    ext2_fs->write_gdt();
    ext2_fs->write_super();
    return new_entry;
}

VFS::File *EXT2_DEntry::get_file() {
    if (type==VFS::SymbolLink)
        return follow()->open();
    return new EXT2_File(this);
}

void EXT2_DEntry::unlink(VFS::DEntry *d) {
    logd("unlink sub current %d, del %d.", inode_n, d->inode_n);
    assert(type == VFS::Directory);
    load_children();
    if (d->type == VFS::Directory) {
        ext2_inode->nlink--;  // 降低自己的引用链接
        ext2_inode->write_inode();
        ext2_fs->gdt_list[d->inode_n / ext2_fs->sb->inodes_per_group]->get_gd()->used_dirs_count--;
        ext2_fs->write_gdt();
    }
    d->unlink();  // 删除子项目
    children.remove(d);
    delete d;
    write_children();
    logd("unlink sub current %d, del %d. finish", inode_n, d->inode_n);
}


void EXT2_DEntry::unlink() {
    inflate();
    logd("ulink %d ref cnt: %d type: %d.", inode_n, ext2_inode->nlink, type);
    if (ext2_inode->nlink == 0)
        return;

    ext2_inode->nlink--;
    // 硬引用计数减一，当引用计数为0时彻底删除文件，目录可以等于1
    if (ext2_inode->nlink == 0 || (type==VFS::Directory && ext2_inode->nlink == 1)) {
        _pos();
        ext2_inode->i->dtime = time(0);
        EXT2_File f(this);
        f.resize(0);
        // if (type==VFS::Directory)
        //     ext2_inode->i->size = 1024;
        ext2_inode->nlink = 0;
        ext2_fs->release_inode(inode_n);
        ext2_fs->write_gdt();
        ext2_fs->write_super();
    }
    ext2_inode->write_inode();
    logd("ulink %d ref cnt: %d type: %d. finish", inode_n, ext2_inode->nlink, type);
}

void EXT2_DEntry::unlink_children() {
    logd("unlink chlidren current %d, del", inode_n);
    assert(type == VFS::Directory);
    load_children();
    _pos();
    for (auto d: children) {
        if (d->inode_n == inode_n || d->inode_n == parent->inode_n)
            continue;
        if (d->type == VFS::Directory) {
            ext2_inode->nlink--;  // 降低自己的引用链接
            ext2_fs->gdt_list[d->inode_n / ext2_fs->sb->inodes_per_group]->get_gd()->used_dirs_count--;
            d->unlink_children();
        }
        d->unlink();  // 删除子项目
        delete d;
    }
    children.clear();
    ext2_inode->write_inode();
    ext2_fs->write_gdt();
    write_children();
    logd("unlink chlidren current %d. finish", inode_n);
}

VFS::DEntry *EXT2_DEntry::link(DEntry *tar, const std::string& s) {
    if (tar->fs != fs) {
        _log_info("[EXT2]: Hard Link must in the same file system.");
        return nullptr;
    }
    if (type == VFS::Directory) {
        _log_info("%s: hard link not allowed for directory", name.c_str());
        return nullptr;
    }
    EXT2_DEntry *t = dynamic_cast<EXT2_DEntry*>(tar);
    std::string ts;
    if (s == "")
        ts = t->name;
    else
        ts = s;
    EXT2_DEntry *new_e = new EXT2_DEntry(ext2_fs, parent, inode_n, type, ts);
    t->load_children();
    t->children.push_back(new_e);
    t->write_children();
    inflate();
    if (type == VFS::Directory) {
        // 目录的硬链接比普通文件多一个
        ext2_inode->nlink += 2;
        // 目录的硬链接，组中目录数目加一
        ext2_fs->gdt_list[inode_n / ext2_fs->sb->inodes_per_group]->get_gd()->used_dirs_count++;
        ext2_fs->write_gdt();
    } else
        ext2_inode->nlink += 1;
    ext2_inode->write_inode();
    return new_e;
}

VFS::DEntry *EXT2_DEntry::move(DEntry *dir, const std::string& new_name) {
    auto ans = link(dir, new_name);  // 在dir中创建item的硬链接
    parent->unlink(this);  // 在父目录中删除自己
    return ans;
}

bool EXT2_DEntry::empty() {
    load_children();
    return children.size() == 2;
}

VFS::DEntry* EXT2_DEntry::copy(DEntry *dir, const std::string& new_name) {
    VFS::DEntry *new_entry = dir->create(new_name==""?this->name:new_name);
    VFS::File *old_f = this->open();
    VFS::File *new_f = new_entry->open();
    char buf[old_f->size];
    old_f->read(buf, old_f->size);
    new_f->write(buf, old_f->size);
    // dynamic_cast<EXT2_DEntry*>(new_entry)->ext2_inode->write_inode();
    delete old_f;
    delete new_f;
    return new_entry;
}

VFS::DEntry *EXT2_DEntry::symlink(DEntry *file, const std::string& new_name) {
    EXT2_DEntry *new_entry = static_cast<EXT2_DEntry*>(create(new_name==""?file->name:new_name));
    _pos();
    new_entry->inode->mode = S_IFLNK | S_IRWXG | S_IRWXO | S_IRWXU;
    new_entry->type = VFS::SymbolLink;
    _pos();
    std::string path = file->printed_path();
    if (path.size() <= 60) {
        memmove(new_entry->ext2_inode->i->block, path.c_str(), path.size());
    } else {
        VFS::File *file = new_entry->open();
        file->write(path.c_str(), path.size());
        file->close();
    }
    new_entry->ext2_inode->size = path.size();
    new_entry->inode->size = path.size();
    new_entry->ext2_inode->write_inode();
    write_children();
    // _d_end();
    return new_entry;
}

VFS::DEntry *EXT2_DEntry::follow() {
    logd("start");
    inflate();
    char buf[inode->size];
    if (inode->size <= 60) {
        memmove(buf, ext2_inode->i->block, inode->size);
    } else {
        VFS::File *f = open();
        f->read(buf, inode->size);
        f->close();
    }
    _ss(buf);
    VFS::NameI *path = VFS::NameI::from_str(std::string(buf, inode->size));
    VFS::DEntry *ans = parent->get_child(path);
    delete path;
    // _d_end();
    return ans;
}


EXT2_DEntry::~EXT2_DEntry() {
    delete inode;
    for (auto x: children) {
        if (x->name != ".." && x->name != ".")
            delete x;
    }
}

