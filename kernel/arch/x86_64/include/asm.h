#pragma once
#include <types.h>
#define asm __asm__ 
#define ASM asm volatile
static inline void outb(u8 v, u16 port) {
	asm volatile("outb %0,%1" : : "a" (v), "dN" (port));
}
static inline u8 inb(u16 port) {
	u8 v;
	asm volatile("inb %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

static inline void outw(u16 v, u16 port) {
	asm volatile("outw %0,%1" : : "a" (v), "dN" (port));
}
static inline u16 inw(u16 port) {
	u16 v;
	asm volatile("inw %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

static inline void outl(u32 v, u16 port) {
	asm volatile("outl %0,%1" : : "a" (v), "dN" (port));
}
static inline u32 inl(u16 port) {
	u32 v;
	asm volatile("inl %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

static inline void outq(u64 v, u16 port) {
	asm volatile("outq %0,%1" : : "a" (v), "dN" (port));
}
static inline u64 inq(u16 port) {
	u64 v;
	asm volatile("inq %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

static inline u8 rdfs8(addr_t addr) {
	u8 v;
	asm volatile("movb %%fs:%1,%0" : "=q" (v) : "m" (*(u8 *)addr));
	return v;
}
static inline u16 rdfs16(addr_t addr) {
	u16 v;
	asm volatile("movw %%fs:%1,%0" : "=r" (v) : "m" (*(u16 *)addr));
	return v;
}
static inline u32 rdfs32(addr_t addr) {
	u32 v;
	asm volatile("movl %%fs:%1,%0" : "=r" (v) : "m" (*(u32 *)addr));
	return v;
}

static inline void wrfs8(u8 v, addr_t addr) {
	asm volatile("movb %1,%%fs:%0" : "+m" (*(u8 *)addr) : "qi" (v));
}
static inline void wrfs16(u16 v, addr_t addr) {
	asm volatile("movw %1,%%fs:%0" : "+m" (*(u16 *)addr) : "ri" (v));
}
static inline void wrfs32(u32 v, addr_t addr) {
	asm volatile("movl %1,%%fs:%0" : "+m" (*(u32 *)addr) : "ri" (v));
}

static inline u8 rdgs8(addr_t addr) {
	u8 v;
	asm volatile("movb %%gs:%1,%0" : "=q" (v) : "m" (*(u8 *)addr));
	return v;
}
static inline u16 rdgs16(addr_t addr) {
	u16 v;
	asm volatile("movw %%gs:%1,%0" : "=r" (v) : "m" (*(u16 *)addr));
	return v;
}
static inline u32 rdgs32(addr_t addr) {
	u32 v;
	asm volatile("movl %%gs:%1,%0" : "=r" (v) : "m" (*(u32 *)addr));
	return v;
}

static inline void wrgs8(u8 v, addr_t addr) {
	asm volatile("movb %1,%%gs:%0" : "+m" (*(u8 *)addr) : "qi" (v));
}
static inline void wrgs16(u16 v, addr_t addr) {
	asm volatile("movw %1,%%gs:%0" : "+m" (*(u16 *)addr) : "ri" (v));
}
static inline void wrgs32(u32 v, addr_t addr) {
	asm volatile("movl %1,%%gs:%0" : "+m" (*(u32 *)addr) : "ri" (v));
}

/* Note: these only return true/false, not a signed return value! */
static inline int memcmp(const void *s1, const void *s2, size_t len) {
	u8 diff;
	asm("repe; cmpsb; setnz %0"
	    : "=qm" (diff), "+D" (s1), "+S" (s2), "+c" (len));
	return diff;
}

static inline int memcmp_fs(const void *s1, addr_t s2, size_t len) {
	u8 diff;
	asm volatile("fs; repe; cmpsb; setnz %0"
	             : "=qm" (diff), "+D" (s1), "+S" (s2), "+c" (len));
	return diff;
}
static inline int memcmp_gs(const void *s1, addr_t s2, size_t len) {
	u8 diff;
	asm volatile("gs; repe; cmpsb; setnz %0"
	             : "=qm" (diff), "+D" (s1), "+S" (s2), "+c" (len));
	return diff;
}

static inline u16 ds(void) {
	u16 seg;
	asm("movw %%ds,%0" : "=rm" (seg));
	return seg;
}

static inline void set_fs(u16 seg) {
	asm volatile("movw %0,%%fs" : : "rm" (seg));
}
static inline u16 fs(void) {
	u16 seg;
	asm volatile("movw %%fs,%0" : "=rm" (seg));
	return seg;
}

static inline void set_gs(u16 seg) {
	asm volatile("movw %0,%%gs" : : "rm" (seg));
}
static inline u16 gs(void) {
	u16 seg;
	asm volatile("movw %%gs,%0" : "=rm" (seg));
	return seg;
}

#define DEFINE_READ_CR(n)                               \
static inline u64 read_cr ## n () {                     \
    u64 x;                                              \
    ASM("mov %%cr" #n ", %0" : "=r"(x));                \
    return x;                                           \
}
#define DEFINE_WRITE_CR(n)                              \
static inline void write_cr ## n (u64 x) {              \
    ASM("mov %0, %%cr" #n :: "r"(x));                   \
}

DEFINE_READ_CR (0)      // read_cr0
DEFINE_READ_CR (2)      // read_cr2
DEFINE_READ_CR (3)      // read_cr3
DEFINE_READ_CR (4)      // read_cr4
DEFINE_WRITE_CR(0)      // write_cr0
DEFINE_WRITE_CR(2)      // write_cr2
DEFINE_WRITE_CR(3)      // write_cr3
DEFINE_WRITE_CR(4)      // write_cr4


// read msr registers
static inline u64 read_msr(u32 msr) {
    union {
        u32 d[2];
        u64 q;
    } u;
    ASM("rdmsr" : "=d"(u.d[1]), "=a"(u.d[0]) : "c"(msr));
    return u.q;
}

// write msr registers
static inline void write_msr(u32 msr, u64 val) {
    union {
        u32 d[2];
        u64 q;
    } u;
    u.q = val;
    ASM("wrmsr" :: "d"(u.d[1]), "a"(u.d[0]), "c"(msr));
}

// special msr registers
static inline u64 read_fsbase() {
    return read_msr(0xc0000100U);
}

static inline u64 read_gsbase() {
    return read_msr(0xc0000101U);
}

static inline u64 read_kgsbase() {
    return read_msr(0xc0000102U);
}

static inline void write_fsbase(u64 val) {
    write_msr(0xc0000100U, val);
}

static inline void write_gsbase(u64 val) {
    write_msr(0xc0000101U, val);
}

static inline void write_kgsbase(u64 val) {
    write_msr(0xc0000102U, val);
}


static inline void die() {
	ASM("hlt");
}

static inline void relax() {
	ASM("pause");
}

static inline u32 int_lock() {
    u64 key;
    ASM("pushfq; cli; popq %0" : "=r"(key));
    return (u32) key & 0x200;
}

static inline void int_unlock(u32 key) {
    if (key & 0x200) {
        ASM("sti");
    }
}
