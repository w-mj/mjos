/* 这个文件不属于该项目，是newlib的一部分
 * 编译newlib时应将该文件复制到newlib/libc/sys/mjos/或建立符号链接 */
#pragma GCC push_options
#pragma GCC optimize ("O0")

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <sys/kstat.h>
//#include <stdint.h>
//#if UINTPTR_MAX == 0xffffffff
//#error "32"
///* 32-bit */
//#elif UINTPTR_MAX == 0xffffffffffffffff
///* 64-bit */
//#error "64"
//#else
//#error "wtf"
///* wtf */
//#endif

// __asm__(".code64");
#define UNIMPLEMENT do {\
	sys_unimplemented_syscall(__func__); \
	while (1); \
} while(0); \
return 0;
 
void _exit() {
	sys_quit_thread();
}
int close(int file) {
	return sys_close(file);
}
char **environ; /* pointer to array of char * strings that define the current environment variables */
int execve(char *name, char **argv, char **env) {
	return sys_exec(name);
}
int fork() {
	UNIMPLEMENT;
}

void copy_st(struct stat *st, kStat *kst) {
    st->st_dev     = kst->dev    ;
    st->st_ino     = kst->ino    ;
    st->st_mode    = kst->mode   ;
    st->st_nlink   = kst->nlink  ;
    st->st_uid     = kst->uid    ;
    st->st_gid     = kst->gid    ;
    st->st_rdev    = kst->rdev   ;
    st->st_size    = kst->size   ;
    st->st_blksize = kst->blksize;
    st->st_blocks  = kst->blocks ;
    st->st_atime   = kst->atime  ;
    st->st_mtime   = kst->mtime  ;
    st->st_ctime   = kst->ctime  ;
}
int fstat(int file, struct stat *st) {
    kStat kst;
    int r = sys_fstat(file, &kst);
    copy_st(st, &kst);
    return r;
}
int getpid() {
	return sys_getpid();
}
int isatty(int file) {
	return (file == 0 || file  == 1 || file == 2);
}

int kill(int pid, int sig) {
	return sys_signal(sig, 0, pid);
}
int link(char *old, char *new) {
	return sys_link(old, new);
}
int lseek(int file, int ptr, int dir) {
	return sys_lseek(file, ptr, dir);
}
int open(const char *name, int flags, ...) {
	return sys_open(name);
}
int read(int file, char *ptr, int len) {
	return sys_read(file, ptr, len);
}
caddr_t sbrk(int incr) {
	return sys_sbrk(incr);
}
int stat(const char *file, struct stat *st) {
    kStat kst;
    int r = sys_stat(file, &kst);
    copy_st(st, &kst);
    return r;
}
clock_t times(struct tms *buf) {
	UNIMPLEMENT;
}
int unlink(char *name) {
	return sys_unlink(name);
}
int wait(int *status) {
	UNIMPLEMENT;
}
int write(int file, char *ptr, int len) {
	return sys_write(file, ptr, len);
}
int gettimeofday (struct timeval *tv, void * tz) {
	UNIMPLEMENT;
}
#pragma GCC pop_options