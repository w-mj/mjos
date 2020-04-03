#include <syscall.h>

int main() {
    sys_write(1, "Hello world\n", 12);
    return 0;
}