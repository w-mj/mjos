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
#include <process/signal.h>
#include <time.h>

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
    char str[] = "write to stdio\n";
    size_t len = strlen(str);
    while (1) {
        for (int i = 0; i < 65536 * 2000; i++) {
            ;
        }
        // sys_read(FD_STDIN, str, len);
        sys_write(FD_STDOUT, str, len);
        sys_print_msg("user message\n");
    }
}
void user_process2() {
//   for (int i = 0; i < 65536 * 2000; i++) {
//       ;
//   }
//   sys_print_msg("user message2\n");
   // sys_signal(SignalType::SIG_KEY, 0, NOPID);
   while(1);
}

bool init_signal_handler(const Signal *signal) {
    logd("Signal!");
    return true;
}


void init_main() {
    os::cout << "cout form " << "iostream" << os::endl;
    logi("start init process");
//    struct tm time_s;
    // time(&time_s);
//    logi("%d-%d-%d %d:%d:%d", time_s.tm_year, time_s.tm_mon, time_s.tm_mday, time_s.tm_hour, time_s.tm_min, time_s.tm_sec);
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
    stdout = new PIPE::PIPE_File();
    stdin  = new PIPE::PIPE_File();
    stderr = stdout;
    ProcessDescriptor* processDescriptor = get_process(do_getpid());
    processDescriptor->fds[FD_STDIN] = stdin;
    processDescriptor->fds[FD_STDOUT] = stdout;
    processDescriptor->fds[FD_STDOUT] = stderr;
    processDescriptor->signalHandler = init_signal_handler;

    // 注册信号
    // signalRegister(SignalType::SIG_KEY, init_signal_handler, SignalRegisterType::NORMAL);
    // do_signal_register(SignalType::SIG_KEY);

    ASM("sti");
    // sys_print_msg("lalala");
    do_create_process(PROCESS_USER, (void*)user_process);
    do_create_process(PROCESS_USER, (void*)user_process2);
    do_create_process_from_file("shell.run");
    do_create_process(PROCESS_KERNEL, (void*)process_print_message);
    while (true);
}
