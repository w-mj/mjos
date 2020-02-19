#pragma once

// 关于系统架构的一些定义

#define KERNEL_CODE_DEC 0x08  // 内核代码段描述符
#define KERNEL_DATA_DEC 0x10  // 内核数据段描述符
#define USER_CODE_DEC   0x2B  // 用户代码段描述符
#define USER_DATA_DEC   0x23  // 用户数据段描述符

#define PAGEOFFSET      12
#define PAGESIZE       (1 << PAGEOFFSET)

#define KERNEL_LMA      0x0000000001000000UL    // 内核的实际位置
#define KERNEL_VMA      0xffffffff81000000UL    // 内核的虚拟地址
#define MAPPED_ADDR     0xffff800000000000UL    // 低端内存的映射地址
#define MAPPED_SIZE     0x0000100000000000UL    // 4K*2^32 = 16TB

#define USER_START      0x0000000000000000UL
#define USER_END        0x0000800000000000UL

#define DMA_START       0x0000000000000000UL
#define DMA_END         0x0000000001000000UL
#define NORMAL_START    0x0000000001000000UL
#define NORMAL_END      0xffffffffffffffffUL
