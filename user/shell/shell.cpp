#include <syscall.h>
#include <process/signal.h>
#include "keycode2ascii.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char buf[1024];

void promote() {
    sys_getcwd(buf, 1024);
    printf("shell %s> ", buf);
    fflush(stdout);
}
int main(int argc, char **argv) {
    printf("=======MJOS START=======\n");
    sys_signal_register(SignalType::SIG_KEY);
    promote();
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
            if (c == '\n') {
                promote();
            }
        }
    }
    return true;
}