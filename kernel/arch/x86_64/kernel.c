#include <attribute.h>
#include <types.h>
#include <console.h>
 
__INIT __NORETURN void kernel_main(void) 
{
	console_initialize();
	console_writez("Hello, kernel World!????\n");
	while(1);
}
