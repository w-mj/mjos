#include <linkage.h>

// void memcpy(_u8 *dst, _u8 *src, _u16 size)
//                  rdi       rsi       rdx
GLOBAL(memcpy)
	pushq %rcx
	xorq  %rcx, %rcx
	movq %rdx, %rcx
	pushq %rcx
	shrq  $3, %rcx
	rep; movsq
	popq  %rcx
	andq  $7, %rcx
	rep; movsq
	popq  %rcx
	ret
ENDPROC(memcpy)
	

//GLOBAL(memcpy)
//    pushw   %si
//    pushw   %di
//    movw    %ax, %di
//    movw    %dx, %si
//    pushw   %cx
//    shrw    $2, %cx
//    rep; movsl
//    popw    %cx
//    andw    $3, %cx
//    rep; movsb
//    popw    %di
//    popw    %si
//    retl
//ENDPROC(memcpy)
//
//
//GLOBAL(memset)
//	pushw	%di
//	movw	%ax, %di
//	movzbl	%dl, %eax
//	imull	$0x01010101,%eax
//	pushw	%cx
//	shrw	$2, %cx
//	rep; stosl
//	popw	%cx
//	andw	$3, %cx
//	rep; stosb
//	popw	%di
//	retl
//ENDPROC(memset)
