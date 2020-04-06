#include <syscall.h>
#include <process/signal.h>
#include <iostream.hpp>

int main() {
    sys_signal_register(SignalType::SIG_KEY);
    sys_write(1, "Hello world\n", 12);
    while(1);
    return 0;
}

bool on_signal(const Signal *signal) {
    if (signal->type == SignalType::SIG_KEY) {
        sys_write(1, "key\r\n", 5);
    }
    return true;
}