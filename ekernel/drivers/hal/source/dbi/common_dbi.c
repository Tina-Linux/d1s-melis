#include <stdio.h>
#include <stdlib.h>
#include <interrupt.h>
#include <sunxi_hal_dbi.h>
#include <hal_cache.h>
#include <hal_mem.h>
#include <hal_osal.h>
#include <hal_log.h>
#include <hal_gpio.h>
#include <hal_dma.h>
#include <hal_reset.h>

#include "platform_dbi.h"

void dbi_print_regs(unsigned long from, unsigned long to)
{
	unsigned int num = (to - from)/16;
	while(num--){
		printf("0x%08lx: ",from);
		printf("%08x %08x %08x %08x\n",
			(*((volatile unsigned int  *)(from))),
			(*((volatile unsigned int  *)(from+4))),
			(*((volatile unsigned int  *)(from+8))),
			(*((volatile unsigned int  *)(from+12)))
		);
		from+=16;
	}
}

void check_reg(void)
{
	dbi_print_regs(0x04026000, 0x04026010);
	*(volatile u32 *) 0x04026008 = 0x0;
	dbi_print_regs(0x04026000, 0x04026010);

}
