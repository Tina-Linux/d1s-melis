#ifndef __IRRX_SUN20IW2_H__
#define __IRRX_SUN20IW2_H__

#if defined(CONFIG_ARCH_RISCV_C906)
#define SUNXI_IRQ_IRADC	65
#elif defined(CONFIG_ARCH_DSP)
#define SUNXI_IRQ_IRADC	50
#else
#define SUNXI_IRQ_IRADC	49
#endif

#define SUNXI_IRADC_PBASE	0x40046000
#define IRADC_PIN		GPIO_PA12
#define IR_MUXSEL		5
#define IR_DRVSEL		2

#endif /* __IRRX_SUN20IW2_H__ */
