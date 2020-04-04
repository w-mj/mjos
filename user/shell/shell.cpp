#include <syscall.h>
#include <process/signal.h>
#include <iostream.hpp>

int main() {
    sys_write(1, "Hello world\n", 12);
    return 0;
}

__attribute__((__section__("kernel")))
bool on_signal(const Signal *signal) {
    if (signal->type == SignalType::SIG_KEY) {
        sys_write(1, "key\n", 5);
    }
    return false;
}