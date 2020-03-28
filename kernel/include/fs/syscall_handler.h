//
// Created by wmj on 3/25/20.
//

#ifndef OS_SYSCALL_HANDLER_H
#define OS_SYSCALL_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

int do_open(const char *);
int do_read(int, char *, size_t);
int do_write(int, const char *, size_t);
int do_close(int);

#ifdef __cplusplus
};
#endif

#endif //OS_SYSCALL_HANDLER_H
