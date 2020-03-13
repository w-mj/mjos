#pragma once

int do_send_message(pid_t pid, const char *str, int size);
int do_read_message(char *dst);
