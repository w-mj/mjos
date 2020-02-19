#pragma once

#ifndef __ASSEMBLER__
void idt_init();
void gdt_init();
void tss_init();
void cpu_init();
#endif

