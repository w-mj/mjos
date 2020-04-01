#include <syscall.h>

int main(void) {
    sys_write(1, "Hello world\n", 12);
}