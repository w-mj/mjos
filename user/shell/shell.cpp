#include <syscall.h>
#include <process/signal.h>
#include "keycode2ascii.h"
#include <stdio.h>

int main() {
    sys_signal_register(SignalType::SIG_KEY);
    sys_write(1, "Hello world\n", 12);
    volatile int a = 0;
//    if (a > 0)
    printf("hello printf\n");
    while(1);
    return 0;
}

bool on_signal(const Signal *signal) {
    char msg[] = "0";
    if (signal->type == SignalType::SIG_KEY) {
        char c = keycode2ascii(static_cast<keycode_t>(signal->value));
        if (c) {
            msg[0] = c;
            sys_write(1, msg, 1);
        }
    }
    return true;
}