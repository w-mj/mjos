#include <attribute.h>
#include <types.h>
#include <console.h>
#include <serial.h>
#include <delog.h>
#include <boot.h>

__INITDATA struct BootParameters bootParameters;

#define TESTTYPE(x) logi("%d %d", (x) / 8, sizeof(u##x))
void test_types(void) {
	TESTTYPE(8);
	TESTTYPE(16);
	TESTTYPE(32);
	TESTTYPE(64);
}

multiboot_info_t* multiboot_info = NULL;
 
__INIT __NORETURN void kernel_main(u64 rax, u64 rbx) 
{
	console_initialize();
	serial_initialize();
	logi("System init finish");
	assert(rax == 0x2BADB002);
	if (rax != 0x2BADB002)  {
		loge("RAX = %x", rax);
	} else {
		multiboot_info = (multiboot_info_t*)rbx;
		logi("mem_lower: %d", ((multiboot_info_t*)rbx) -> mem_lower);
		logi("mem_upper: %d", ((multiboot_info_t*)rbx) -> mem_upper);
	}
	test_types();
	if (bootParameters.memoryListLength > 0) {
		bootParameters.memoryListLength -= 4;
		bootParameters.memoryListLength /= 24;
		logi("Memoey list:");
		for (u32 i = 0; i < bootParameters.memoryListLength; i++) {
			logi("%d %d %d %d", 
					bootParameters.memoryList[i].address,
					bootParameters.memoryList[i].length,
					bootParameters.memoryList[i].type,
					bootParameters.memoryList[i].addition
					);
		}
	} else {
		loge("No memory detected.");
	}
	while(1);
}
