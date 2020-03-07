#pragma once

int send_message(pid_t pid, char *str, int size);
int read_message(char *dst);
