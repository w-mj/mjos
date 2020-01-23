#include <attribute.h>
#include <types.h>
#include <console.h>
#include <serial.h>
#include <delog.h>
 
__INIT __NORETURN void kernel_main(void) 
{
	console_initialize();
	serial_initialize();
	logi("System init finish");
	while(1);
}
