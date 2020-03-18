#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#define asm __asm__ 
#define ASM asm volatile
#define DEFINE_IN(n, suffix)                            \
static inline u ## n in ## n (u16 p) {                  \
    u ## n x;                                           \
    ASM("in" suffix " %1, %0" : "=a"(x) : "Nd"(p));     \
    return x;                                           \
}
#define DEFINE_OUT(n, suffix)                           \
static inline void out ## n (u##n x, u16 p) {         \
    ASM("out" suffix " %0, %1" :: "a"(x), "Nd"(p));     \
}

DEFINE_IN (8,  "b")     // in8
DEFINE_IN (16, "w")     // in16
DEFINE_IN (32, "l")     // in32
DEFINE_OUT(8,  "b")     // out8
DEFINE_OUT(16, "w")     // out16
DEFINE_OUT(32, "l")     // out32
/*
#define DEFINE_OUT(n, suffix) \
static inline void out##n(u##n v, u16 port) {\
	asm volatile("out"#suffix" %0,%1" : : "a" (v), "dN" (port)); \
}
#define DEFINE_IN(n, suffix)\
static inline u##n in##n(u16 port) { \
	u##n v; \
	asm volatile("in"#suffix" %1,%0" : "=a" (v) : "dN" (port)); \
	return v; \
} \

DEFINE_IN (8,  b)
DEFINE_IN (16, w)
DEFINE_IN (32, l)
DEFINE_OUT(8,  b)
DEFINE_OUT(16, w)
DEFINE_OUT(32, l)
*/
#define DEFINE_READ(n) \
static inline u##n read##n (void * p) {\
    return *((u##n volatile *) p); \
}
#define DEFINE_WRITE(n) \
static inline void write##n (u##n x, void *p)  { \
    *((u##n volatile*)p) = x; \
}

DEFINE_READ (8) 
DEFINE_READ (16)
DEFINE_READ (32)
DEFINE_READ (64)
DEFINE_WRITE(8) 
DEFINE_WRITE(16)
DEFINE_WRITE(32)
DEFINE_WRITE(64)


#define DEFINE_READ_SEG(seg, n, suf) \
static inline u##n rd##seg##n(addr_t addr) { \
	u##n v; \
	ASM("mov"#suf" %%"#seg":%1, %0" : "=q"(v):"m"(*(u##n*)addr));\
	return v; \
}

DEFINE_READ_SEG(fs, 8,  b)
DEFINE_READ_SEG(fs, 16, w)
DEFINE_READ_SEG(fs, 32, l)
DEFINE_READ_SEG(gs, 8,  b)
DEFINE_READ_SEG(gs, 16, w)
DEFINE_READ_SEG(gs, 32, l)

static inline void wrfs8(u8 v, addr_t addr) {
	asm volatile("movb %1,%%fs:%0" : "+m" (*(u8 *)addr) : "qi" (v));
}
static inline void wrfs16(u16 v, addr_t addr) {
	asm volatile("movw %1,%%fs:%0" : "+m" (*(u16 *)addr) : "ri" (v));
}
static inline void wrfs32(u32 v, addr_t addr) {
	asm volatile("movl %1,%%fs:%0" : "+m" (*(u32 *)addr) : "ri" (v));
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
	ASM("cli");
	ASM("hlt");
}

static inline void relax() {
	ASM("pause");
}

// 关中断，返回值指示调用int_lock前IF的状态
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

#ifdef __cplusplus
}
#endif
