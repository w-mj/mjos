#include <process/process.h>
#include <fs/vfs.hpp>
#include <string.hpp>
#include <memory/kmalloc.h>
#include <cpu.h>

extern "C" void copy_file(ProcessDescriptor *to, ProcessDescriptor *from) {
    for (int i = 0; i < CFG_PROCESS_FDS; i++) {
        if (from->fds[i] != NULL) {
            to->fds[i] = from->fds[i];
            static_cast<VFS::File*>(to->fds[i])->open();
        }
    }
}
