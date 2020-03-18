#pragma once
#ifdef __cplusplus
extern "C" {
#endif


#ifndef __ASSEMBLER__
void idt_init();
void gdt_init();
void tss_init();
void cpu_init();
void per_cpu_init();
#endif


#ifdef __cplusplus
}
#endif
