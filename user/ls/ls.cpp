#include <syscall.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int fd;
    if (argc == 1) {
        fd = open(".", O_RDONLY);
    } else if (argc == 2) {
        fd = open(argv[1], O_RDONLY);
    }
    if (fd == -1) {
        printf("can not open directory.");
        return -1;
    }
    char buf[1024];
    while (sys_getdent(fd, buf, 1024)) {
        puts(buf);
    }
    return 0;
}