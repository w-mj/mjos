#include <base.h>
#include <delog.h>
#include <asm.h>
#include <cpu.h>
#include <early_kmalloc.h>
#include <biosreg.h>
#include <process/process.h>
#include <boot.h>
#include <string.h>
#include <syscall.h>

__PERCPU ThreadDescriptor *tid_prev = NULL,
                         *tid_next = NULL;  // 在线程切换时由prev切换至next

extern u64 _percpu_end, _percpu_addr;

int cpu_installed;
int cpu_activated;
u64 percpu_base;
u64 percpu_size;

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

#define IA32_EFER  0xC0000080 
#define IA32_STAR  0xC0000081
#define IA32_LSTAR 0xC0000082
#define IA32_FMASK 0xC0000084

void syscall_stub();
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
		logi("cpu vendor %s", vendor_id);
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
	u64 efer = read_msr(IA32_EFER);
	efer |= (1UL <<  0);
	write_msr(IA32_EFER, efer);  // 打开syscall
	write_msr(IA32_LSTAR, (u64)syscall_stub);  // syscall 地址
	write_msr(IA32_FMASK, 0);   // rflags = rflags & ~(FMASK)
	u64 star = ((u64)KERNEL_CODE_DEC << 32) | ((u64)(USER_CODE_DEC) << 48);
	write_msr(IA32_STAR, star);  // 设置段选择子
}

extern u64 _percpu_addr, _percpu_end;
void per_cpu_init() {
	if (cpu_activated == 0) {
		logd("per_cpu_init %d cpu(s)", cpu_count());
		percpu_size = &_percpu_end - &_percpu_addr;
		percpu_size = ROUND_UP(percpu_size, 64);
		void *percpu_area = early_kmalloc(cpu_count() * percpu_size);
		percpu_base = (u64)(percpu_area - (u64)&_percpu_addr);
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
	// _sL(read_gsbase());
	return (void *) ((char *) ptr + read_gsbase());
}


void prepare_switch(ThreadDescriptor *prev, ThreadDescriptor *next) {
	// logd("prepare switch from %d to %d", prev->process->pid, next->process->pid);
	thiscpu_var(tid_prev) = prev;
	thiscpu_var(tid_next) = next;
}
