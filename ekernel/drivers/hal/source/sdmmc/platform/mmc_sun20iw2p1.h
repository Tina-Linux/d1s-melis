#ifndef __MMC_SUN8IW19_H__
#define __MMC_SUN8IW19_H__

#include <hal_gpio.h>

//#define SDC_FPGA

#define SMC0_BASE                       (0x40008000)
#define SMC1_BASE                       (0x04021000)

#define SDC_CCM_BASE                    (0x4003c000)
#define SDC_GPIO_BASE                   (SUNXI_GPIO_PBASE)

#ifdef CONFIG_ARCH_RISCV_C906
#define __GIC_SRC_MMC0        87 // (78)
#define __GIC_SRC_MMC1        87 // (79)
#else
#define __GIC_SRC_MMC0        71 // (78)
#define __GIC_SRC_MMC1        71 // (79)
#endif

#define  SDC0_IRQn  __GIC_SRC_MMC0
#define  SDC1_IRQn  __GIC_SRC_MMC1

#define SDMMC_MUXSEL 2
#define SDMMC_DRVSEL 3
#define SDMMC_PULL GPIO_PULL_DOWN_DISABLE

#define SDC_PLL_CLK (192*1000*1000)

#define SDC_CCM_SDC0_SCLK_CTRL          (SDC_CCM_BASE + 0x28)
#define SDC_CCM_SClk_DIV_N_SHIFT   (16)

#define SDC_CCM_SDC_BUS_GATE		(SDC_CCM_BASE + 0x4)
#define SDC_CCM_SDC_BUS_RESET		(SDC_CCM_BASE + 0xc)


#ifdef CONFIG_PLATFORM_FPGA 
/*sdc0 pin*/
#define SDC0_NUM 6
#define SDC0_CLK    GPIO_PA29
#define SDC0_CMD    GPIO_PA25
#define SDC0_D0    GPIO_PA26
#define SDC0_D1    GPIO_PA27
#define SDC0_D2    GPIO_PA28
#define SDC0_D3    GPIO_PA24
#define SDC0_DET    GPIO_PA15

/*sdc1 pin*/
#define SDC1_NUM 6
#define SDC1_CLK    GPIO_PA0
#define SDC1_CMD    GPIO_PA1
#define SDC1_D0    GPIO_PA2
#define SDC1_D1    GPIO_PA3
#define SDC1_D2    GPIO_PA4
#define SDC1_D3    GPIO_PA5

#define SDC_FPGA

#else
//unfine in ic
#endif
#define SDC_DES_ADDR_SHIFT (2)

#endif
