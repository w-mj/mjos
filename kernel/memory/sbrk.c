#include <process/process.h>
#include <memory/page.h>
#include <cpu.h>

void *do_sbrk(int inc) {
    ProcessDescriptor *process = thiscpu_var(current)->process;
    void *to_ret = process->heap_end;
    logd("sbrk pid:%d, heap end:%x, linear end%x, inc:%x", process->pid, process->heap_end, process->linear_end, inc);
    process->heap_end += inc;   // 修改堆终止地址
    if (process->heap_end > process->linear_end) {
        // 堆空间大于线性区空间，尝试分配页面
        long mem_size = process->heap_end - process->linear_end;
        mem_size = ROUND_UP(mem_size, PAGESIZE);
        void *mem_end= process->linear_end + mem_size;  // 新的线性区结束地址
        int pages = (int)(mem_size >> PAGEOFFSET);   // 需申请的页面数
        void *new_mem = normal_pages_alloc(process->cr3, (u64)process->linear_end, pages);
        assert(new_mem == process->linear_end);  // 保证申请的空间与之前地址空间连续
        process->linear_end = mem_end;
    } else if (process->linear_end - process->heap_end >= PAGESIZE) {
        // 堆的大小比线性区小超过一个页面，尝试回收多余的页面
        void *mem_end = (void*)ROUND_UP((u64)process->heap_end, PAGESIZE);  // 新的线性区结束地址
        int pages = (int)((process->linear_end - mem_end) >> PAGEOFFSET);
        normal_pages_release(mem_end, pages, process->cr3);
        process->linear_end = mem_end;
    }
    return to_ret;
}