#pragma once

#include "types.h"
namespace EXT2
{
    
constexpr _u16 VALID_FS = 0x01; // 文件系统状态定义，没有正常卸载
constexpr _u16 ERROR_FS = 0x02; // 文件系统状态定义，内核代码检测到错误
/**
 * ext2 文件系统的超级块定义
 */
struct SuperBlock {
    _u32 inodes_count; // 文件系统中Inode数量
    _u32 blocks_count; // 文件系统中总块数
    _u32 r_blocks_count; // 文件系统中保留块数目
    _u32 free_blocks_count; // 文件系统中空闲块数目
    _u32 free_inodes_count; // 文件系统中空闲Inode数量
    _u32 first_data_block; // 第一个数据块的位置，通常当数据块大小为1k时为1，否则为0
    _u32 log_block_size; // 块大小，以2的幂次方表示，1024字节为单位。
    _i32 log_frag_size; // 逻辑片大小，用于实现块片，即将一个块分成多个片，以保存多个文件的数据
    _u32 blocks_per_group; // 每组块数
    _u32 frags_per_group; // 每组碎片数
    _u32 inodes_per_group; // 每组Inode数
    _u32 mtime; // 上次挂载时间，为1970-1-1到现在的秒数
    _u32 wtime; // 上次写入超级块的时间
    _u16 mnt_count; // 未进行检查的挂载次数
    _i16 max_mnt_count; // 必须执行检查前的最大挂载次数（可为-1）
    _u16 magic; // Magic signature 用于表示文件系统的版本，一般为0xEF53
    _u16 state; // 文件系统的状态，包括VALID_FS和ERROR_FS的或值。
    _u16 errors; // 表明出现错误时要执行的操作
    _u16 pad; // 未使用
    _u32 lastcheck; // 上一次执行检查的时间
    _u32 checkinterval; // 两次执行检查的间隔时间
    _u32 creator_os;
    _u32 revuvel;
    _u16 def_resuid;
    _u16 def_resgid;

    _u32 first_ino; /* First non-reserved inode */
    _u16 inode_size; /* size of inode structure */
    _u16 block_group_nr; /* block group # of this superblock */
    _u32 feature_compat; /* compatible feature set */
    _u32 feature_incompat; /* incompatible feature set */
    _u32 feature_ro_compat; /* readonly-compatible feature set */
    _u8 uuid[16]; /* 128-bit uuid for volume */

    _u8 volume_name[16]; /* volume name */
    _u8 last_mounted[64]; /* directory where last mounted */

    _u32 algorithm_useage_bitmap; // 用于压缩

    _u8 prealloc_blocks; /* Nr of blocks to try to preallocate*/
    _u8 prealloc_dir_blocks; /* Nr to preallocate for dirs */
    _u16 padding1;

    _u32 reserved[204]; /* Padding to the end of the block */
};
/**
 * ext2的组描述符
 */
struct GroupDescriptor {
    _u32 block_bitmap; // 块位图的块号
    _u32 inode_bitmap; // 索引节点位图的块号
    _u32 inode_table; // 第一个索引节点表块的块号
    _u16 free_blocks_count; // 组中空闲块的个数
    _u16 free_inodes_count; // 组中空闲索引节点的个数
    _u16 used_dirs_count; // 组中目录的个数
    _u16 padding;
    _u32 reversed[3];
};

constexpr int NDIR_BLOCKS = 12;
constexpr int IND_BLOCK = NDIR_BLOCKS;
constexpr int DIND_BLOCK = IND_BLOCK + 1;
constexpr int TIND_BLOCK = DIND_BLOCK + 1;
constexpr int N_BLOCKS = TIND_BLOCK + 1;

/**
 * ext2的文件索引
 */
union InodeOsd1 {
    struct {
        _u32 l_i_reserved1;
    } linux1;
    struct {
        _u32 h_i_translator;
    } hurd1;
    struct {
        _u32 m_i_reserved1;
    } masix1;
};
union InodeOsd2 {
    struct {
        _u8 l_i_frag; /* Fragment number */
        _u8 l_i_fsize; /* Fragment size */
        _u16 i_pad1;
        _u16 l_i_uid_high; /* these 2 fields    */
        _u16 l_i_gid_high; /* were reserved2[0] */
        _u32 l_i_reserved2;
    } linux2;
    struct {
        _u8 h_i_frag; /* Fragment number */
        _u8 h_i_fsize; /* Fragment size */
        _u16 h_i_mode_high;
        _u16 h_i_uid_high;
        _u16 h_i_gid_high;
        _u32 h_i_author;
    } hurd2;
    struct {
        _u8 m_i_frag; /* Fragment number */
        _u8 m_i_fsize; /* Fragment size */
        _u16 m_pad1;
        _u32 m_i_reserved2[2];
    } masix2;
};
struct Inode {
    _u16 mode; // 文件类型和访问权限
    _u16 uid; // 拥有者
    _u32 size; // 文件长度
    _u32 atime; // 最后一次访问时间
    _u32 ctime; // 索引节点最后改变的时间
    _u32 mtime; // 文件内容最后改变的时间
    _u32 dtime; // 文件删除的时间
    _u16 gid; // 组
    _u16 links_count; // 硬链接计数器
    _u32 blocks; // 文件的数据块数
    _u32 flags; // 文件标志
    InodeOsd1 osd1; /* OS dependent 1 */
    _u32 block[N_BLOCKS]; // 指向数据块的指针
    _u32 generation; // 文件版本
    _u32 file_acl; // 文件访问控制列表
    _u32 dir_acl; // 目录访问控制列表
    _u32 faddr; // 片地址
    InodeOsd2 osd2; /* OS dependent 2 */
};

/**
 * ext2中目录结构
 */
constexpr int NAME_LEN = 255;

struct DirEntry {
	_u32 inode;  // 目标的索引节点号
	_u16 rec_len;  // 目录项长度  当前项起始位置+目录项长度=下一个目录项位置
	_u8 name_len;  // 文件名长度
	_u8 file_type;  // 文件类型
	_u8 name[NAME_LEN];  // 文件名
};
} // namespace EXT2
