#ifndef __CONFIG_SL_SUN8IW19P1_H_
#define __CONFIG_SL_SUN8IW19P1_H_

#include <sunxi_hal_regulator.h>
#include <hal_gpio.h>
#include "../vin.h"
#include "../vin-video/vin_core.h"

unsigned int vin_csi_base[VIN_MAX_CSI] = {
	CSI0_REGS_BASE,
	CSI1_REGS_BASE
};

unsigned int vin_isp_base[VIN_MAX_ISP] = {
	ISP_REGS_BASE
};

unsigned int vin_isp_irq[VIN_MAX_ISP] = {
	SUNXI_IRQ_ISP0
};

unsigned int vin_mipi_base[VIN_MAX_MIPI] = {
	MIPI_REGS_BASE
};

unsigned int vin_scaler_base[VIN_MAX_SCALER] = {
	VIPP0_REGS_BASE,
	VIPP1_REGS_BASE,
	VIPP2_REGS_BASE,
	VIPP3_REGS_BASE
};

struct vin_clk_info vind_default_clk[VIN_MAX_CLK] = {
	[VIN_TOP_CLK] = {
		.clock = HAL_CLK_PERIPH_CSI_TOP,
#if defined CONFIG_VIN_SENSOR_imx386
		.frequency = 336000000,
#elif defined CONFIG_VIN_SENSOR_C2398
		.frequency = 300000000,
#endif
	},
	[VIN_TOP_CLK_SRC] = {
		.clock = HAL_CLK_PLL_CSI,
	},
};

struct vin_mclk_info vind_default_mclk[VIN_MAX_CCI] = {
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER0,
		.clk_24m = HAL_CLK_SRC_HOSC24M,
		.clk_pll = HAL_CLK_PLL_CSI,
		.pin = {
			[0] = {0, GPIOI(0), 2, 0, 2},
			[1] = {0, GPIOI(0), 7, 0, 2},
		},
	},
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER1,
		.clk_24m = HAL_CLK_SRC_HOSC24M,
		.clk_pll = HAL_CLK_PLL_CSI,
		.pin = {
			[0] = {0, GPIOE(1), 2, 0, 2},
			[1] = {0, GPIOE(1), 7, 0, 2},
		},
	},
};

struct vin_clk_info vind_default_isp_clk[VIN_ISP_MAX_CLK] = {
	[VIN_ISP_CLK] = {
		.clock = HAL_CLK_PERIPH_ISP,
		.frequency = 300000000,
	},
	[VIN_ISP_CLK_SRC] = {
		.clock = HAL_CLK_PLL_PERI1,
	},
};

#if 0
struct vin_clk_info vind_default_mipi_clk[VIN_MIPI_MAX_CLK] = {
	[VIN_MIPI_CLK] = {
		.clock = HAL_CLK_UNINITIALIZED,
	},
	[VIN_MIPI_CLK_SRC] = {
		.clock = HAL_CLK_UNINITIALIZED,
	},
};
#endif

struct vin_core sunxi_vinc[VIN_MAX_DEV] = {
	[0] = {
		.id = 0,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 0,
		.mipi_sel = 0,
		.isp_sel = 0,
		.base = (void *)CSI_DMA0_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA0,
	},
	[1] = {
		.id = 1,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 0,
		.mipi_sel = 0,
		.isp_sel = 0,
		.base = (void *)CSI_DMA1_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA1,
	},
	[2] = {
		.id = 2,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 0,
		.mipi_sel = 0,
		.isp_sel = 0,
		.base = (void *)CSI_DMA2_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA2,
	},
	[3] = {
		.id = 3,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 0,
		.mipi_sel = 0,
		.isp_sel = 0,
		.base = (void *)CSI_DMA3_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA3,
	}
};

struct sensor_list sensors_default[VIN_MAX_DEV] = 
{
    {
        .use_sensor_list = 0,
        .used = 0,
        .csi_sel = 0,
        .device_sel = 0,
        .mclk_id = 0,
        .sensor_bus_sel = 1,
        .sensor_bus_type = 0,
        .act_bus_sel = 0,
        .act_bus_type = 0,
        .act_separate = 0,
        .power_set = 0,
        .detect_num = 1,
        .sensor_pos = "rear",
        .valid_idx = 0,
        .power = 
        {
            [IOVDD] = {NULL, AXP2101_ID_ALDO2, 1800000, "iovdd"},
            [AVDD] = {NULL, AXP2101_ID_BLDO2, 2800000, "avdd"},
            [DVDD] = {NULL, AXP2101_ID_DLDO2, 1200000, "dvdd"},
            [AFVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [FLVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [CAMERAVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
        },
        .gpio = 
        {
            [POWER_EN] = {0, GPIO_INDEX_INVALID, 0, 0, 0},
            [PWDN] = {0, GPIOI(4), 1, 0, 1},
            [RESET] = {0, GPIOI(3), 1, 0, 1},
            [SM_HS] = {0, GPIO_INDEX_INVALID, 0, 0, 0},
            [SM_VS] = {0, GPIO_INDEX_INVALID, 0, 0, 0},
            [AF_PWDN] = {0, GPIO_INDEX_INVALID, 0, 0, 0},
            [FLASH_EN] = {0, GPIO_INDEX_INVALID, 0, 0, 0},
            [FLASH_MODE] = {0, GPIO_INDEX_INVALID, 0, 0, 0},
        },
        .inst = 
        {
#if defined CONFIG_VIN_SENSOR_imx386
            [0] = 
                {
                    .cam_name = "imx386_mipi",
                    .cam_addr = 0x20,
                    .cam_type = 1,
                    .is_isp_used = 1,
                    .is_bayer_raw = 1,
                    .vflip = 0,
                    .hflip = 0,
                    .act_addr = 0x0,
                    .act_name = "",
                    .isp_cfg_name = "",
                },
#elif defined CONFIG_VIN_SENSOR_C2398
            [0] = 
                {
                    .cam_name = "C2398_mipi",
                    .cam_addr = 0x6c,
                    .cam_type = 1,
                    .is_isp_used = 1,
                    .is_bayer_raw = 1,
                    .vflip = 0,
                    .hflip = 0,
                    .act_addr = 0x0,
                    .act_name = "",
                    .isp_cfg_name = "",
                },
#endif
        },
    },
};
#endif
