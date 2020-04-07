#include <syscall.h>
#include <process/signal.h>
#include "keycode2ascii.h"

int main() {
    sys_signal_register(SignalType::SIG_KEY);
    sys_write(1, "Hello world\n", 12);
    while(1);
    return 0;
}

bool on_signal(const Signal *signal) {
    char msg[] = "key.\r\n";
    if (signal->type == SignalType::SIG_KEY) {
        char c = keycode2ascii(static_cast<keycode_t>(signal->value));
        if (c) {
            msg[3] = c;
            sys_write(1, msg, 6);
        }
    }
    return true;
}