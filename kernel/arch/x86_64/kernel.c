#include <attribute.h>
#include <types.h>
#include <console.h>
#include <copy.h>
#include <delog.h>
 
__INIT __NORETURN void kernel_main(void) 
{
	console_initialize();
	console_writez("start\r\n");
	char *s1 = "000original string123\r\n";
	//char s2[128] = {0};
	//memset(s2, 'c', 10);
	//memset(s2 + 10, 't', 10);
	//s2[20] = '\r';
	//s2[21] = '\n';
	//s2[22] = '\0';
	//console_writez(s2);
	//console_writez(s1);
	for (int i = 0; i < 200; i++) {
		int a = i;
		s1[2] = a % 10 + '0';
		a = a / 10;
		s1[1] = a % 10 + '0';
		a = a / 10;
		s1[0] = a + '0';
		console_writez(s1);
	}
	_si(1);
	_sa(s1, 100);
	logi("This is %s", "info");
	logw("This is warring %d", 1);
	loge("This is error %d", 1);
	while(1);
}
