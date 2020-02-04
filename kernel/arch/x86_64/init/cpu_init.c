#include <types.h>
#include <string.h>
#include <base.h>
#include <delog.h>

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
}
