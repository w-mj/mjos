#include <syscall.h>
extern int main();
void _start() {
    main();
    sys_quit_thread();
}