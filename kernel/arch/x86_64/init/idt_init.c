#include <types.h>
#include <delog.h>
#include <asm.h>
#include <attribute.h>
#include <base.h>

struct IDTR {
	u16 size;
	u64 address;
}__PACKED;
struct Attr {
	u16	ist	 : 3,
		zero : 5,
		type : 5,
		dpl	 : 2,
		p	 : 1;
}__PACKED;
struct ID {
	u16 offset_low;
	u16 segment;
	struct Attr attr;
	u16 offset_mid;
	u32 offset_high;
	u32 pad;
}__PACKED;


static void handle_divide_zero() {
	loge("Divide zero");
	die();
}

static void handle_page_fault() {
	loge("Page fault");
	die();
}
#define IDT_LENGTH 256
struct ID idt[IDT_LENGTH];
struct IDTR idtr;

static void set_id(int i, u64 handle) {
	idt[i].offset_low  = (u16)(u64)handle;
	idt[i].segment     = 8;
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
void load_idtr(struct IDTR *);
void idt_init() {
	logi("idt init");
	// set_id(0,  handle_divide_zero);
	// set_id(14, handle_page_fault);
	int entry_size = ((u64)&interrupt_stub_entry_end - (u64)&interrupt_stub_entry) / IDT_LENGTH;
	_si(entry_size);
	for (int i = 0; i < IDT_LENGTH; i++) {
		set_id(i, ((u64)(&interrupt_stub_entry)) + i * entry_size);
	}
	idtr.size = sizeof(struct ID) * IDT_LENGTH - 1;
	idtr.address = (u64)idt;
	asm volatile("lidt %0"::"m" (idtr));
	asm volatile("sti");
	// asm volatile("int $14");
	// asm volatile("int $100");
	// asm volatile("int $0");
	logi("idt init finish");
	logi("idt init finish");
}

void interrupt_stub(u64 vec, u64 errcode) {
	loge("interrupt %d errcode 0x%x%08x.", vec, h32(errcode), l32(errcode));
	u64 rsp;
	asm volatile("movq %%rsp, %0": "=m"(rsp));
	// _sa((void*)rsp, 256);
	while(1);
	die();
}

