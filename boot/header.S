.global _start
.code16

.section .text

_start:
    mov $0x1301, %ax
    mov $0x000f, %bx
    mov $msg_len, %cx
    mov $msg, %bp

    int $0x10
_pause:
    jmp _pause

msg:
    .ascii "Hello, world\r\n"
    .equ msg_len, . - msg
    .org 0x200 - 2
    .word 0xaa55
