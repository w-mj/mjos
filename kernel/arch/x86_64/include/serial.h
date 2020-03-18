#pragma once
#ifdef __cplusplus
extern "C" {
#endif


void serial_initialize();
void  serial_putchar(char a);
char serial_getchar();

#ifdef __cplusplus
}
#endif
