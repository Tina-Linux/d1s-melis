#ifndef __IRRX_SUN55IW3_H__
#define __IRRX_SUN55IW3_H__

#include <hal_interrupt.h>

#if defined(CONFIG_ARCH_RISCV)
#define SUNXI_IRQ_IRADC		MAKE_IRQn(71, 0)
#endif

#define SUNXI_IRADC_PBASE	0x07040000
#define IRADC_PIN		GPIOB(21)
#define IR_MUXSEL		2
#define IR_DRVSEL		2

#endif /* __IRRX_SUN55IW3_H__ */
