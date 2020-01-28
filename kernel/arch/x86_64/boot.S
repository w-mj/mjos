
#define MB_MAGIC    0x1badb002
#define MB_FLAGS    1<<0 | 1<<1
#define MB_CHECK    (- (MB_MAGIC + MB_FLAGS))

#define KERNEL_LMA  0x0000000001000000
#define KERNEL_VMA  0xffffffff81000000
#define ABSOLUTE(x) ((x) - KERNEL_VMA + KERNEL_LMA)
#include <linkage.h>
GLOBAL_FUNC(_start)
EXTERN_FUNC(kernel_main)  // 六十四位内核程序入口
EXTERN_FUNC(init16)  // 十六位实模式C语言程序入口

//------------------------------------------------------------------------------
// first piece of code to run, 32-bit protected mode, paging disabled

.section .boot, "ax"
.code32

load:
    jmp     _start

.balign 4, 0
mb_header:
    .long   MB_MAGIC
    .long   MB_FLAGS
    .long   MB_CHECK

.balign 8, 0
_start:
	movw $0xffffffff, %sp
	call init16
    // disable interrupts and save GRUB parameters
    cli  // 关中断
    //movl    %eax, (mb_eax)
    //movl    %ebx, (mb_ebx)
	cld  // 正向

    // 关分页
    movl    %cr0, %eax
    andl    $0x7fffffff, %eax
    movl    %eax, %cr0

    // 读入临时页表
    movl    $ABSOLUTE(pml4), %edi
    movl    %edi, %cr3

    // 打开PAE模式
    movl    %cr4, %eax
    orl     $1 << 5, %eax
    movl    %eax, %cr4

    // 启动长模式
    // 启动长模式后可以获得真64位数据寄存器以及一些额外的寄存器。
    movl    $0xc0000080, %ecx
    rdmsr
    orl     $1 <<  8, %eax
    wrmsr

    // 开启分页
    movl    %cr0, %eax
    orl     $1 << 31, %eax
    movl    %eax, %cr0

    // 段地址为1
    lgdt    gdtr64
    jmp     $8, $entry64

die32:
    hlt
    jmp     die32

//------------------------------------------------------------------------------
// start running under 64-bit mode (lower half)

.code64

entry64:
    // first initialize all segment registers
	// 10 000b 全部指向索引为2的段，即data0 
    movw    $0x10, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss

    // clear FS.base and GS.base
	// FS和GS是两个段寄存器，在64bit模式下，其高32位存放在MSR中
    xorq    %rax, %rax
    movl    $0xc0000100, %ecx
    wrmsr
    movl    $0xc0000101, %ecx
    wrmsr

    // jump to the last 2GB of vm space
    jmp     higher_half

die64_low:
    hlt
    jmp     die64_low

.balign 16
gdt64:
    .quad   0           // dummy descriptor
    .word   0           // code 0: limit [0 :15]
    .word   0           //         base  [0 :15]
    .byte   0           //         base  [16:23]
    .byte   0x98        //         Present, DPL=0, non-conforming
    .byte   0xa0        //         64-bit, and limit [16:19]
    .byte   0           //         base  [24:31]
    .word   0           // data 0: limit [0 :15]
    .word   0           //         base  [0 :15]
    .byte   0           //         base  [16:23]
    .byte   0x92        //         Present, DPL=0, writable
    .byte   0xc0        //         attr and limit [16:19]
    .byte   0           //         base  [24:31]
/* gdt中定义了3个段，每个段描述符8字节，0号段为dummy descriptor
 * gdtr64 的前2字节是段表长度，有3个段因此长度为3*8-1
 * 段号的前3位是描述属性，第4位开始是段序号
 * jmp $8, $entry64中的$8指定的是1号段，即code0
*/

gdtr64:
    .word   3 * 8 - 1
    .quad   gdt64

mb_eax:
    .long   0
mb_ebx:
    .long   0

//------------------------------------------------------------------------------
// init code section, in full 64-bit mode

.section .init.text, "ax"
.code64

higher_half:
    // set initial kernel stack
    movq    $boot_stack_top, %rsp
    xorq    %rbp, %rbp

    // clear rflags (again)
    xorq    %rax, %rax
    pushq   %rax
    popfq

    // begin executing C code in higher-half
    // movl    (mb_eax), %eax
    // movl    (mb_ebx), %edi
	jmp kernel_main

die64_high:
    hlt
    jmp     die64_high

//------------------------------------------------------------------------------
// init data section, temporary page table and kernel stack

.section .init.data, "aw"

.balign 0x1000
pml4:
    .quad   ABSOLUTE(pdp0) + 7          // pml4[0]   -> pdp0
    .skip   255 * 8
    .quad   ABSOLUTE(pdp0) + 7          // pml4[256] -> pdp0
    .skip   254 * 8
    .quad   ABSOLUTE(pdp1) + 7          // pml4[511] -> pdp1

.balign 0x1000
pdp0:
    .quad   ABSOLUTE(pd + 0x0000) + 7   // pdp0[0]   -> pd0
    .quad   ABSOLUTE(pd + 0x1000) + 7   // pdp0[1]   -> pd1
    .quad   ABSOLUTE(pd + 0x2000) + 7   // pdp0[2]   -> pd2
    .quad   ABSOLUTE(pd + 0x3000) + 7   // pdp0[3]   -> pd3

.balign 0x1000
pdp1:
    .skip   510 * 8
    .quad   ABSOLUTE(pd + 0x0000) + 7   // pdp1[510] -> pd0
    .quad   ABSOLUTE(pd + 0x1000) + 7   // pdp1[511] -> pd1

.balign 0x1000
pd:
index = 0
.rept 512 * 4
    .quad   (index << 21) + 0x87        // 2MB page, US, RW
    index = index + 1
.endr

.balign 0x1000
boot_stack:
    .skip   0x4000
boot_stack_top:

// .section .bss
// .code
// .balign 0x1000
// e_stack:
// 	.skip 0x4000
// e_stack_top:
