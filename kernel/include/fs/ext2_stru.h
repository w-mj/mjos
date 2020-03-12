#pragma once
#include <types.h>
typedef struct SuperBlock {
    u32 inodes_count; // 文件系统中Inode数量
    u32 blocks_count; // 文件系统中总块数
    u32 r_blocks_count; // 文件系统中保留块数目
    u32 free_blocks_count; // 文件系统中空闲块数目
    u32 free_inodes_count; // 文件系统中空闲Inode数量
    u32 first_data_block; // 第一个数据块的位置，通常当数据块大小为1k时为1，否则为0
    u32 log_block_size; // 块大小，以2的幂次方表示，1024字节为单位。
    i32 log_frag_size; // 逻辑片大小，用于实现块片，即将一个块分成多个片，以保存多个文件的数据
    u32 blocks_per_group; // 每组块数
    u32 frags_per_group; // 每组碎片数
    u32 inodes_per_group; // 每组Inode数
    u32 mtime; // 上次挂载时间，为1970-1-1到现在的秒数
    u32 wtime; // 上次写入超级块的时间
    u16 mnt_count; // 未进行检查的挂载次数
    i16 max_mnt_count; // 必须执行检查前的最大挂载次数（可为-1）
    u16 magic; // Magic signature 用于表示文件系统的版本，一般为0xEF53
    u16 state; // 文件系统的状态，包括VALID_FS和ERROR_FS的或值。
    u16 errors; // 表明出现错误时要执行的操作
    u16 pad; // 未使用
    u32 lastcheck; // 上一次执行检查的时间
    u32 checkinterval; // 两次执行检查的间隔时间
    u32 creator_os;
    u32 revuvel;
    u16 def_resuid;
    u16 def_resgid;

    u32 first_ino; /* First non-reserved inode */
    u16 inode_size; /* size of inode structure */
    u16 block_group_nr; /* block group # of this superblock */
    u32 feature_compat; /* compatible feature set */
    u32 feature_incompat; /* incompatible feature set */
    u32 feature_ro_compat; /* readonly-compatible feature set */
    u8 uuid[16]; /* 128-bit uuid for volume */

    u8 volume_name[16]; /* volume name */
    u8 last_mounted[64]; /* directory where last mounted */

    u32 algorithm_useage_bitmap; // 用于压缩

    u8 prealloc_blocks; /* Nr of blocks to try to preallocate*/
    u8 prealloc_dir_blocks; /* Nr to preallocate for dirs */
    u16 padding1;

    u32 reserved[204]; /* Padding to the end of the block */
} ext2_sb;