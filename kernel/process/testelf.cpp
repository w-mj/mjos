extern "C" {
#include <syscall.h>
}

int main() {
    sys_print_msg("from CPP");
}
