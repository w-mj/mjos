/* 这个文件不属于该项目，是newlib的一部分
 * 编译newlib时应将该文件复制到newlib/libc/sys/mjos/或建立符号链接 */
#pragma GCC push_options
#pragma GCC optimize ("O0")
#include <fcntl.h>
#include <syscall.h>

extern void exit(int code);
extern int main(int argc, char **argv);
extern void _init();
 
void _start() {
	_init();
	int argc = 0;
	char **argv = NULL;
	sys_build_args(&argc, &argv);
	exit(main(argc, argv));
}
#pragma GCC pop_options
