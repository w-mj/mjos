#pragma once
#ifdef __cplusplus
extern "C" {
#endif


int do_send_message(pid_t pid, const char *str, int size);
int do_read_message(char *dst);

#ifdef __cplusplus
}
#endif
