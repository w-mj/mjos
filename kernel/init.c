#include <syscall.h>
#include <string.h>
#include <console.h>
#include <serial.h>
#include <delog.h>
#include <cpu.h>

void process_print_message() {
    logd("start print message process");
    do_func_register(SYS_FUNC_PRINTMSG);
    char buf[1024];
    while (1) {
        int size = do_read_message(buf);
        if (size == 0)
            continue;
        for (int i = 0; i < size; i++) {
            char c = buf[i];
            if (c == '\n') {
                console_putchar('\r');
                serial_putchar('\r');
            }
            console_putchar(c);
            serial_putchar(c);
        }
    }
}

void user_process() {
    while (1) {
        for (int i = 0; i < 65536 * 2000; i++) {
            ;
        }
        sys_print_msg("user message\n");
    }
}
void user_process2() {
   for (int i = 0; i < 65536 * 2000; i++) {
       ;
   }
   sys_print_msg("user message2\n");
}

void init_main() {
    logi("start init process");
    ASM("sti");
    // sys_print_msg("lalala");
    do_create_process(PROCESS_USER, user_process);
    do_create_process(PROCESS_USER, user_process2);
    do_create_process(PROCESS_KERNEL, process_print_message);
    while (1);
}
