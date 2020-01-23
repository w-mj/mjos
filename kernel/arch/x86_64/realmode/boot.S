
#define MB_MAGIC    0x1badb002
#define MB_FLAGS    1<<0 | 1<<1
#define MB_CHECK    (- (MB_MAGIC + MB_FLAGS))

#define KERNEL_LMA  0x0000000001000000
#define KERNEL_VMA  0xffffffff81000000
#define ABSOLUTE(x) ((x) - KERNEL_VMA + KERNEL_LMA)

#include <linkage.h>
GLOBAL_FUNC(_start)
EXTERN_FUNC(init16)  // 十六位实模式C语言程序入口


.section .boot, "ax"
.code16

load:
    jmp     _start

.balign 4, 0
mb_header:
    .long   MB_MAGIC
    .long   MB_FLAGS
    .long   MB_CHECK

.balign 8, 0
_start:
    cli  // 关中断
	cld  // 正向

    // 关分页
    movl    %cr0, %eax
    andl    $0x7fffffff, %eax
    movl    %eax, %cr0

	movw	$hello_msg, %si
msg_loop:
	lodsb
	andb	%al, %al
	jz      pnt_over
	movb	$0xe, %ah
	movw	$7, %bx
	int		$0x10
	jmp	msg_loop
pnt_over:

	movw    $0x10, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss
	
	movw	$boot_stack_top, %sp
    jmp init16 

.section .init.data, "aw"
hello_msg: 
	.asciiz "Start up operating system\r\n"
.balign 0x1000
boot_stack:
    .skip   0x4000
boot_stack_top:

