#include <syscall.h>
#include <process/signal.h>
#include "keycode2ascii.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <fs/ext2/ext2_disk_stru.hpp>

int main() {
    sys_signal_register(SignalType::SIG_KEY);
    char buf[512];
    sprintf(buf, "hello %s %d", "?", 123);
    sys_write(1, buf, strlen(buf));
    write(1, "Hello world\n", 12);
    // fork();
    printf("hello printf\n");
    sys_chdir("/usr/include");
    sys_getcwd(buf, 512);
    printf("cwd : %s\n", buf);
    sys_chdir("../");
    int fd = open(".", O_RDONLY);
    while (sys_getdent(fd, buf, 512)) {
        puts(buf);
    }
    close(fd);
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