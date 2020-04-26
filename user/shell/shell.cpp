#include <syscall.h>
#include <process/signal.h>
#include "keycode2ascii.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char buf[1024];
char command[1024];
int cmd_cur = 0;

void promote() {
    sys_getcwd(buf, 1024);
    printf("shell %s> ", buf);
    fflush(stdout);
}

void parse_cmd() {
    command[cmd_cur] = '\0';
    char *space = strchr(command, ' ');
    if (space) {
        *space = '\0';
        sprintf(buf, "/usr/bin/%s.run %s", command, space + 1);
    } else {
        sprintf(buf, "/usr/bin/%s.run", command);
    }
    pid_t pid = sys_create_process_from_file(buf);
    sys_waitpid(pid);
}

int main(int argc, char **argv) {
    printf("=======MJOS START=======\n");
    sys_signal_register(SignalType::SIG_KEY);
    promote();
    int len;
    char c;
    while(1) {
        if ((len = sys_read(0, buf, 1024))) {
            for (int i = 0; i < len; i++) {
                c = buf[i];
                putchar(c);
                if (c != '\n') {
                    command[cmd_cur++] = c;
                } else {
                    parse_cmd();
                    cmd_cur = 0;
                    promote();
                }
            }
            fflush(stdout);
        }
    }
    return 0;
}

bool on_signal(const Signal *signal) {
    if (signal->type == SignalType::SIG_KEY) {
        char c = keycode2ascii(static_cast<keycode_t>(signal->value));
        if (c) {
            sys_write(0, &c, 1);
        }
    }
    return true;
}