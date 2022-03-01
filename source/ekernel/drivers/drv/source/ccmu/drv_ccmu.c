#include <init.h>
#include <rtthread.h>
#include <log.h>
#include <hal_clk.h>

int drv_clock_init(void)
{
    hal_clock_init();
	return 0;
}
