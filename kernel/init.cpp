#include <syscall.h>
#include <console.h>
#include <serial.h>
#include <delog.h>
#include <cpu.h>
#include <fs/vfs.hpp>
#include <fs/ext2/ext2_fs.hpp>
#include <iostream.hpp>
#include <dev/sata_dev.hpp>
#include <process/scheduler.h>
#include <fs/pipe/pipe.hpp>

extern "C" void init_main();

void process_print_message() {
    logd("start print message process");
    do_func_register(SYS_FUNC_PRINTMSG);
    char buf[1024];
    while (true) {
        int size = do_read(FD_STDOUT, buf, 1024);
        if (size == 0) {
            size = do_read_message(buf);
        }
        if (size == 0)
            sched_yield();
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
    const char *str = "write to stdio\n";
    size_t len = strlen(str);
    while (1) {
        for (int i = 0; i < 65536 * 2000; i++) {
            ;
        }
        sys_write(FD_STDOUT, str, len);
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
    os::cout << "cout form " << "iostream" << os::endl;
    logi("start init process");
    // parse_elf64(user_processes[0]);
    // die();
    // 加载文件系统
    auto dev = new Dev::SataDev;
    auto fs = new EXT2::EXT2_FS(dev);
    root_fs = fs;
    fs->mount();
    auto root = fs->root;
    root->load_children();
    _si(root->children.size());
    os::cout << root->name << os::endl;
//    for (auto x = root->children.begin(); x != root->children.end(); x++) {
//        os::cout << (*x)->name << os::endl;
//    }
    for (auto x: root->children) {
        os::cout << x->name << os::endl;
    }
    // 打开基本文件
    auto stdout = new PIPE::PIPE_File();
    auto stdin  = new PIPE::PIPE_File();
    ProcessDescriptor* processDescriptor = get_process(do_getpid());
    processDescriptor->fds[FD_STDIN] = stdin;
    processDescriptor->fds[FD_STDOUT] = stdout;
    processDescriptor->fds[FD_STDOUT] = stdout;
    // die();
    ASM("sti");
    // sys_print_msg("lalala");
    do_create_process(PROCESS_USER, (void*)user_process);
    do_create_process(PROCESS_USER, (void*)user_process2);
    do_create_process(PROCESS_KERNEL, (void*)process_print_message);
    while (true);
}
