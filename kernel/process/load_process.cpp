// 从磁盘加载进程

#include <string.hpp>
#include <fs/vfs.hpp>
#include <arch.h>
#include <process/process.h>
#include <cpu.h>

extern "C" void exec(const char *path) {
    ProcessDescriptor *process = thiscpu_var(current)->process;

}