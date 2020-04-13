#include <types.h>
#include <delog.h>
#include <asm.h>
#include <base.h>
#include <cpu.h>
#include <arch.h>
#include <init.h>

struct IDTR {
	u16 size;
	u64 address;
} __PACKED;
struct Attr {
	u16	ist	 : 3,
	    zero : 5,
	    type : 5,
	    dpl	 : 2,
	    p	 : 1;
} __PACKED;
struct ID {
	u16 offset_low;
	u16 segment;
	struct Attr attr;
	u16 offset_mid;
	u32 offset_high;
	u32 pad;
} __PACKED;

#define IDT_LENGTH 256
struct ID idt[IDT_LENGTH];

static void set_id(int i, u64 handle) {
	idt[i].offset_low  = (u16)(u64)handle;
	idt[i].segment     = KERNEL_CODE_DEC;
	idt[i].attr.ist    = 0;    // default stack
	idt[i].attr.zero   = 0;    // zero
	idt[i].attr.type   = 0xE;  // GATE_INTERRUPT
	idt[i].attr.dpl    = 0x0;  // DPL = 0
	idt[i].attr.p      = 1;    // P = 1
	idt[i].offset_mid  = (u16)((u64)handle >> 16);
	idt[i].offset_high = (u32)((u64)handle >> 32);
	idt[i].pad         = 0;
}

extern u64 interrupt_stub_entry;
extern u64 interrupt_stub_entry_end;
void *isr_tbl[256];
void interrupt_stub(InterruptContext *cxt);
void load_idtr(struct IDTR *);
void idt_init() {
	if (cpu_activated == 0) {
		logi("idt init");
		// set_id(0,  handle_divide_zero);
		// set_id(14, handle_page_fault);
		int entry_size = ((u64)&interrupt_stub_entry_end - (u64)&interrupt_stub_entry) / IDT_LENGTH;
		_si(entry_size);
		for (int i = 0; i < IDT_LENGTH; i++) {
			set_id(i, ((u64)(&interrupt_stub_entry)) + i * entry_size);
		}
		for (int i = 0; i < 256; i++) {
			isr_tbl[i] = (void*)interrupt_stub;
		}
	}
	struct IDTR idtr;
	idtr.size = sizeof(struct ID) * IDT_LENGTH - 1;
	idtr.address = (u64)idt;
	// logd("idt address %llx", idtr.address);
	asm volatile("lidt %0"::"m" (idtr));
	// u64 page_fault_addr = 0xFFFFF0FFFFFFFFFF;
	// ASM("movq %rax, (0xFFFFF0FFFFFFFFFF)");
	// asm volatile("sti");
	// while (1);
	// asm volatile("int $14");
	// asm volatile("int $100");
	// asm volatile("int $0");
	// logi("idt init finish");
	// logi("idt init finish");
}

void interrupt_stub(InterruptContext *cxt) {
	loge("interrupt %d errcode 0x%x%08x", cxt->vec, h32(cxt->errcode), l32(cxt->errcode));
	loge("RAX=0x%-16llx RBX=0x%-16llx", cxt->rax, cxt->rbx);
	loge("RCX=0x%-16llx RDX=0x%-16llx", cxt->rcx, cxt->rdx);
	loge("RDI=0x%-16llx RSI=0x%-16llx", cxt->rdi, cxt->rsi);
	loge("RBP=0x%-16llx R8 =0x%-16llx", cxt->rbp, cxt->r8);
	loge("R9 =0x%-16llx R10=0x%-16llx", cxt->r9,  cxt->r10);
	loge("R11=0x%-16llx R12=0x%-16llx", cxt->r11, cxt->r12);
	loge("R13=0x%-16llx R14=0x%-16llx", cxt->r13, cxt->r14);
	loge("R15=0x%-16llx", cxt->r15);
	loge("RIP=0x%-16llx CS =0x%-16llx", cxt->rip, cxt->cs);
	loge("RSP=0x%-16llx SS =0x%-16llx", cxt->rsp, cxt->ss);
	loge("RFLAGS=0x%-16llx", cxt->rlfags);
	if (cxt->vec == 14) {
		u64 cr2 = read_cr2();
		// _sL(cr2);
		loge("cr2=%llx", cr2);
	} else if (cxt->vec == 15) {
		u64 code;
		ASM("movq %%r11, %0": "=rm"(code));
		loge("code=%llx", code);
	}
	// _sa((void*)rsp, 256);
	while (1);
	die();
}

