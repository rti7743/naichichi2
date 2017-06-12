/* 強制リブート 

このコードは GPL でライセンスされています。
*/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

int main()
{
	int retry = 0;
	for(retry = 0 ; retry < 1000 ; retry ++ )
	{
		int sysrq = open("/proc/sys/kernel/sysrq", O_RDWR | O_SYNC);
		if (sysrq < 0)
		{
			puts("can not open /proc/sys/kernel/sysrq");
			usleep(1000); //1msec sleep...
			continue;
		}
		write(sysrq,"1",1);
		close(sysrq);

		int sysrqtrigger = open("/proc/sysrq-trigger", O_RDWR | O_SYNC);
		if (sysrqtrigger < 0)
		{
			puts("can not open /proc/sysrq-trigger");
			usleep(1000); //1msec sleep...
			continue;
		}
		write(sysrqtrigger,"b",1);
		close(sysrqtrigger);
	}
	return 0;
}
