#include <types.h>
#include <string.h>
#include <base.h>
#include <delog.h>
#include <asm.h>
#include <cpu.h>
#include <early_kmalloc.h>

extern u64 _percpu_end, _percpu_addr;
u64 percpu_base;

int cpu_installed;
int cpu_activated;
u64 percpu_base;
u64 percpu_size;
void *isr_tbl[256];

static inline void cpuid(u32 *a, u32 *b, u32 *c, u32 *d) {
	u32 eax, ebx, ecx, edx;
	if (NULL == a) {
		eax = 0;
		a   = &eax;
	}
	if (NULL == b) {
		ebx = 0;
		b   = &ebx;
	}
	if (NULL == c) {
		ecx = 0;
		c   = &ecx;
	}
	if (NULL == d) {
		edx = 0;
		d   = &edx;
	}
	ASM("cpuid" : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
	    :  "a"(*a),  "b"(*b),  "c"(*c),  "d"(*d));
}

void cpu_init() {
	if (cpu_activated == 0) {
		char vendor_id[16];
		u32 a, b, c, d;
		// get cpu vendor id string
		a = 0;
		cpuid(&a, &b, &c, &d);
		memcpy(&vendor_id[0], &b, sizeof(u32));
		memcpy(&vendor_id[4], &d, sizeof(u32));
		memcpy(&vendor_id[8], &c, sizeof(u32));
		vendor_id[12] = 0;
		logi("cpu vecdor %s", vendor_id);
		a = 0x80000008;
		cpuid(&a, &b, &c, &d);
		logi("Physical address bits %d", a & 0xff);
		logi("Linear address bits %d", (a >> 8) & 0xff);
	}
	u64 cr0 = read_cr0();
	cr0 |=  (1UL <<  1);        // cr0.MP
	cr0 &= ~(1UL <<  2);        // cr0.EM: disable emulated mode
	cr0 |=  (1UL <<  5);        // cr0.NE: enable native exception
	cr0 |=  (1UL << 16);        // cr0.WP: enable write protection
	write_cr0(cr0);
	// enable syscall/sysret on intel processors
	// u64 efer = read_msr(0xc0000080U);
	// efer |= (1UL <<  0);
	// write_msr(0xc0000080U, efer);
}

extern u64 _percpu_addr, _percpu_end;
void per_cpu_init() {
	if (cpu_activated == 0) {
		logd("per_cpu_init %d cpu(s)", cpu_count());
		percpu_size = &_percpu_end - &_percpu_addr;
		percpu_size = ROUND_UP(percpu_size, 64);
		void *percpu_area = early_kmalloc(cpu_count() * percpu_size);
		percpu_base = (u64)(percpu_area - (void*)&_percpu_addr);
		for (int i = 0; i < cpu_count(); i++) {
			memcpy(percpu_area, &_percpu_addr, percpu_size);
			percpu_area += percpu_size;
		}
	}
	write_gsbase(percpu_base + cpu_activated * percpu_size);
	// _sL(percpu_base);
	// _sL(read_gsbase());
	// _si(cpu_index());
}

int cpu_count() {
	return cpu_installed;
}

int cpu_index() {
	// _sL(read_gsbase());
	// _sL(percpu_base);
	return (int) ((read_gsbase() - percpu_base) / percpu_size);
}

void *calc_percpu_addr(u32 cpu, void *ptr) {
	return (void *) ((char *) ptr + percpu_base + percpu_size * cpu);
}

void *calc_thiscpu_addr(void *ptr) {
	return (void *) ((char *) ptr + read_gsbase());
}
