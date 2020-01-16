#include <attribute.h>
#include <types.h>
#include <console.h>
#include <copy.h>
 
__INIT __NORETURN void kernel_main(void) 
{
	console_initialize();
	console_writez("start\r\n");
	char *s1 = "original string\r\n123";
	char s2[128] = {0};
	memset(s2, 'c', 10);
	memset(s2 + 10, 't', 10);
	s2[20] = '\r';
	s2[21] = '\n';
	s2[22] = '\0';
	console_writez(s2);
	console_writez(s1);
	while(1);
}
