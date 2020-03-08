#include <process/process.h>
#include <spin.h>
#include <string.h>
#include <cpu.h>

int send_message(pid_t pid, const char *str, int size) {
    ProcessDescriptor *target = get_process(pid);
    if (target->shared_mem == NULL)
        return -1;
    irq_spin_take(&target->shared_mem_write_lock);  // P(write)
    memcpy(target->shared_mem + sizeof(int), str, size);
    *(int*)(target->shared_mem) = size;
    irq_spin_give(&target->shared_mem_read_lock, 0x20); // V(read)
    return size;
}

int read_message(char *dst) {
    ProcessDescriptor *process = thiscpu_var(current)->process;
    int *size = (int *)process->shared_mem;
    char *mem = process->shared_mem + sizeof(int);
    irq_spin_take(&process->shared_mem_read_lock);
    if (*size == 0) {
        irq_spin_give(&process->shared_mem_read_lock, 0x20);
        irq_spin_give(&process->shared_mem_write_lock, 0x20);
        return 0;
    }
    memcpy(dst, mem, *size);
    irq_spin_give(&process->shared_mem_write_lock, 0x20);
    return *size;
}