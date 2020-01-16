#include <attribute.h>
#include <types.h>
#include <console.h>
#include <copy.h>
 
__INIT __NORETURN void kernel_main(void) 
{
	char *s1 = "original string\n";
	char s2[128] = {0};
	memcpy(s2, s1, 17);

	console_initialize();
	console_writez(s2);
	while(1);
}
