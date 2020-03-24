#include <types.h>
#include <asm.h>
#include <console.h>
#include <serial.h>
#include <delog.h>
#include <boot.h>
#include <memory/page.h>
#include <memory/slab.h>
#include <memory/kmalloc.h>
#include <init.h>
#include <acpi.h>
#include <cpu.h>
#include <loapic.h>
#include <ioapic.h>
#include <early_kmalloc.h>
#include <process/process.h>
#include <process/scheduler.h>
#include <syscall.h>
#include <spin.h>
#include <stdio.h>
#include <dev/pci.h>
#include <fs/ext2_stru.h>
#include <dev/ahci.h>

#define TESTTYPE(x) assert((x) / 8 == sizeof(u##x))
void test_types(void) {
	TESTTYPE(8);
	TESTTYPE(16);
	TESTTYPE(32);
	TESTTYPE(64);
	HBA_FIS *fis = (HBA_FIS *)0;
	assert((u64)&fis->psfis == 0x20);
}

char map[4096] = {0};

multiboot_info_t *multiboot_info = NULL;
#define GET_BYTE(x, a) (((x) & (0xff << (a << 3))) >> (a << 3))
// 打印从GRUB得到的系统信息
void print_sys_info(void) {
	const multiboot_info_t *mbi = multiboot_info;
	if (mbi->flags & MULTIBOOT_INFO_MEMORY) {
		logi("mem_lower: %dKB", mbi->mem_lower);
		logi("mem_upper: %dKB", mbi->mem_upper);
	}
	if (mbi->flags & MULTIBOOT_INFO_BOOTDEV) {
		logi("boot_device drive: 0x%x", GET_BYTE(mbi->boot_device, 3));
		logi("boot_device part1: 0x%x", GET_BYTE(mbi->boot_device, 2));
		logi("boot_device part2: 0x%x", GET_BYTE(mbi->boot_device, 1));
		logi("boot_device part3: 0x%x", GET_BYTE(mbi->boot_device, 0));
	}
	if (mbi->flags & MULTIBOOT_INFO_CMDLINE) {
		logi("cmdline: %s", mbi->cmdline);
	}
	if (mbi->flags & MULTIBOOT_INFO_MODS) {
		// const multiboot_module_t *mod_addr = (multiboot_module_t*)mbi->mods_addr;
		// for (u32 i = 0; i < mbi->mods_count; i++) {
		// 	logi("boot mods %d: %s", i, mod_addr[i].cmdline);
		// }
		logi("mods_count: %d", mbi->mods_count);
		logi("mods_addr:  %d", mbi->mods_addr);
	}
	if (mbi->flags & MULTIBOOT_INFO_AOUT_SYMS) {
		logi("aout sym addr: %d", mbi->u.aout_sym.addr);
		logi("aout sym tabsize: %d", mbi->u.aout_sym.tabsize);
		logi("aout sym strsize: %d", mbi->u.aout_sym.strsize);
		logi("aout sym reserved: %d", mbi->u.aout_sym.reserved);
	}
	if (mbi->flags & MULTIBOOT_INFO_ELF_SHDR) {
		logi("elf sec addr: %d", mbi->u.elf_sec.addr);
		logi("elf sec num: %d", mbi->u.elf_sec.num);
		logi("elf sec shndx: %d", mbi->u.elf_sec.shndx);
		logi("elf sec size: %d", mbi->u.elf_sec.size);
	}
	if (mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		logi("mmap length: %d", mbi->mmap_length);
		logi("mmap addr: %d", mbi->mmap_addr);
		multiboot_memory_map_t *mmap;
		for (mmap = (multiboot_memory_map_t *)(u64)mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (multiboot_memory_map_t *)((unsigned long ) mmap + mmap->size + sizeof(mmap->size)))
			logi(" size: 0x%x, base_addr: 0x%x%08x, length: 0x%x%08x, type: 0x%x",
					(unsigned) mmap->size,
					(unsigned) (mmap->addr >> 32),
					(unsigned) (mmap->addr & 0xffffffff),
					(unsigned) (mmap->len >> 32),
					(unsigned) (mmap->len & 0xffffffff),
					(unsigned) mmap->type);
	}
	if (mbi->flags & MULTIBOOT_INFO_DRIVE_INFO) {
		logi("drives length: %d", mbi->drives_length);
		logi("drives addr:   %d", mbi->drives_addr);
	}
	if (mbi->flags & MULTIBOOT_INFO_CONFIG_TABLE) {
		logi("config table: %d", mbi->config_table);
	}
	if (mbi->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) {
		logi("boot loader name %s", (char *)(u64)mbi->boot_loader_name);
	}
	if (mbi->flags & MULTIBOOT_INFO_APM_TABLE) {
		// logi("apm table: %d", mbi->apm_table);
	}
}

static __INIT void parse_madt(madt_t * tbl) {
    u8 * end = (u8 *) tbl + tbl->header.length;
    u8 * p   = (u8 *) tbl + sizeof(madt_t);
    cpu_installed = 0;
    cpu_activated = 0;

    // loapic_override(tbl->loapic_addr);
    while (p < end) {
        acpi_subtbl_t * sub = (acpi_subtbl_t *) p;
        switch (sub->type) {
        case MADT_TYPE_IO_APIC:
            ioapic_dev_add((madt_ioapic_t *) sub);
            break;
        case MADT_TYPE_INTERRUPT_OVERRIDE:
            ioapic_int_override((madt_int_override_t *) sub);
            break;
        case MADT_TYPE_LOCAL_APIC:
            loapic_dev_add((madt_loapic_t *) sub);
            break;
        case MADT_TYPE_LOCAL_APIC_OVERRIDE:
            loapic_override(((madt_loapic_override_t *) sub)->address);
            break;
        case MADT_TYPE_LOCAL_APIC_NMI:
            loapic_set_nmi((madt_loapic_mni_t *) sub);
            break;
        default:
			logw("unrecognized madt type %d", sub->type);
            break;
        }
        p += sub->length;
    }
}

void init_main();
void load_tid_next(ThreadDescriptor *);
void __libc_start_main();
// extern void *elf_addr;
extern u64 _bss_end;
__INIT __NORETURN void kernel_main(u64 rax, u64 rbx) {
	console_initialize();
	serial_initialize();
	assert(rax == MULTIBOOT_BOOTLOADER_MAGIC);
	multiboot_info = (multiboot_info_t *)rbx;
	print_sys_info();
	test_types();

	acpi_tbl_init();
	parse_madt(acpi_madt);

	u64 kernel_code_end = (u64)&_bss_end;
	early_kmalloc_init(kernel_code_end);


	cpu_init();
	gdt_init();
	per_cpu_init();
	idt_init();
	tss_init();

	page_init((void*)(u64)multiboot_info->mmap_addr, multiboot_info->mmap_length);
	mem_pool_init();

	kmalloc_init();

    loapic_dev_init();
    ioapic_all_init();
    pci_probe_all();
	process_init();
	scheduler_init();

	// ioapic_gsi_unmask(ioapic_irq_to_gsi(1));
//    ioapic_gsi_unmask(ioapic_irq_to_gsi(16));
//    ioapic_gsi_unmask(ioapic_irq_to_gsi(17));
//    ioapic_gsi_unmask(ioapic_irq_to_gsi(18));
//    ioapic_gsi_unmask(ioapic_irq_to_gsi(19));

    // ext2_sb *sb = (ext2_sb*)kmalloc_s(sizeof(ext2_sb));
    // bool t = sata_read(sata,2, 0, 2, virt_to_phys(sb));
//    if (t == false)
//        loge("read sata error");
//    if (sb->magic == 0xEF53) {
//        logd("ext2 success");
//    } else {
//        logd("Ext2 fail");
//    }
//    kfree_s(sizeof(ext2_sb), sb);

    logi("System init finish");
	// parse_elf64(&elf_addr);
	// die();
	// ASM("movq $0x12, %r11");
	// ASM("int $15");
	__libc_start_main();  // 初始化libc和cxxrt


	pid_t pid = create_process(NULL, PROCESS_KERNEL, (void*)init_main);

	ProcessDescriptor *pd = get_process(pid);
	// assert(pd->cr3 == read_cr3());
	ListEntry *thread_list_entry = pd->threads.next;
	assert(thread_list_entry != &pd->threads);
	ThreadDescriptor *thread = list_entry(thread_list_entry, ThreadDescriptor, sibling);
	assert(thread->process == pd); // 启动init进程
	thiscpu_var(current) = thread;
	set_rsp0((u64) thread->rsp0);
	// 0x0xffffffff8101d3fa
	// 0xffffd8
	// 0x1000008
//	usize tt;
//	tt = virt_to_phys((void*)0xffffffff8101d3fa);
//	_sL(tt);
//	tt = page_translate(read_cr3(),0xffffffff8101d3fa);
//	_sL(tt);
//    tt = virt_to_phys((void*)0xffffd8);
//    _sL(tt);
//    tt = virt_to_phys((void*)0x1000000);
//    _sL(tt);
    // *(u64*)0x0ff0ffff8101d3fa = 1;
    load_tid_next(thread);

	die();
	while (1);
}


