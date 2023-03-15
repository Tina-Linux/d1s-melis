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

int vin_isp_base[VIN_MAX_ISP] = {
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

unsigned int vin_dma_base[VIN_MAX_DEV] = {
	CSI_DMA0_REG_BASE,
	CSI_DMA1_REG_BASE,
	CSI_DMA2_REG_BASE,
	CSI_DMA3_REG_BASE,
};

int vin_dma_irq[VIN_MAX_DEV] = {
	SUNXI_IRQ_CSIC_DMA0,
	SUNXI_IRQ_CSIC_DMA1,
	SUNXI_IRQ_CSIC_DMA2,
	SUNXI_IRQ_CSIC_DMA3,
};

struct vin_clk_info vind_default_clk[VIN_MAX_CLK] = {
	[VIN_TOP_CLK] = {
		.clock = HAL_CLK_PERIPH_CSI_TOP,
#ifndef CONFIG_DRIVER_SYSCONFIG
#if defined CONFIG_VIN_SENSOR_imx386
		.frequency = 336000000,
#elif defined CONFIG_VIN_SENSOR_C2398
		.frequency = 300000000,
#elif defined CONFIG_VIN_SENSOR_gc2053
		.frequency = 324000000,
#endif
#endif
	},
	[VIN_TOP_CLK_SRC] = {
		.clock = HAL_CLK_PLL_CSI,
	},
};

struct vin_mclk_info vind_default_mclk[VIN_MAX_CCI] = {
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER0,
#if defined CONFIG_CSI_PLL_CLK_SPREAD_SPECTRUM
		.clk_24m = HAL_CLK_PLL_CSI,
#else
		.clk_24m = HAL_CLK_SRC_HOSC24M,
#endif
		.clk_pll = HAL_CLK_PLL_CSI,
		.pin = {
			[0] = {0, GPIOI(0), 2, 0, 2},
			[1] = {0, GPIOI(0), 7, 0, 2},
		},
	},
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER1,
#if defined CONFIG_CSI_PLL_CLK_SPREAD_SPECTRUM
		.clk_24m = HAL_CLK_PLL_CSI,
#else
		.clk_24m = HAL_CLK_SRC_HOSC24M,
#endif
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
#ifndef CONFIG_DRIVER_SYSCONFIG
		.frequency = 300000000,
#endif
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

/*
	"csi1_pck", "csi1_hsync", "csi1_vsync",
	"csi1_d0", "csi1_d1", "csi1_d2", "csi1_d3",
	"csi1_d4", "csi1_d5", "csi1_d6", "csi1_d7",
	"csi1_d8", "csi1_d9", "csi1_d10", "csi1_d11",
	"csi1_d12", "csi1_d13", "csi1_d14", "csi1_d15";
*/
int vind_csi_parallel_pins[VIN_MAX_CSI][19] = {
#ifdef CONFIG_DRIVER_SYSCONFIG
	GPIO_INDEX_INVALID
#else
	{
		GPIO_INDEX_INVALID
	},
	{
		GPIOE(0),
		GPIOE(2),
		GPIOE(3),
		GPIOE(4),
		GPIOE(5),
		GPIOE(6),
		GPIOE(7),
		GPIOE(8),
		GPIOE(9),
		GPIOE(10),
		GPIOE(11),
		GPIOE(12),
		GPIOE(13),
		GPIOE(14),
		GPIOE(15),
		GPIOE(18),
		GPIOE(19),
		GPIOE(20),
		GPIOE(21)
	}
#endif
};

struct vin_core sunxi_vinc[VIN_MAX_DEV] = {
#ifdef CONFIG_DRIVER_SYSCONFIG
	0
#else
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
#endif	/*CONFIG_DRIVER_SYSCONFIG*/
};

struct RT_WEAK sensor_list sensors_default[VIN_MAX_DEV] =
{
#ifdef CONFIG_DRIVER_SYSCONFIG
    {
        .power =
        {
            [IOVDD] = {NULL, AXP2101_ID_ALDO2, 0, "iovdd"},
            [AVDD] = {NULL, AXP2101_ID_BLDO2, 0, "avdd"},
            [DVDD] = {NULL, AXP2101_ID_DLDO2, 0, "dvdd"},
            [AFVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [FLVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [CAMERAVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
        },
    },
    {
        .power =
        {
            [IOVDD] = {NULL, AXP2101_ID_ALDO2, 0, "iovdd"},
            [AVDD] = {NULL, AXP2101_ID_BLDO2, 0, "avdd"},
            [DVDD] = {NULL, AXP2101_ID_DLDO2, 0, "dvdd"},
            [AFVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [FLVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [CAMERAVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
        },
    },
    {
        .power =
        {
            [IOVDD] = {NULL, AXP2101_ID_ALDO2, 0, "iovdd"},
            [AVDD] = {NULL, AXP2101_ID_BLDO2, 0, "avdd"},
            [DVDD] = {NULL, AXP2101_ID_DLDO2, 0, "dvdd"},
            [AFVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [FLVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [CAMERAVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
        },
    },
    {
        .power =
        {
            [IOVDD] = {NULL, AXP2101_ID_ALDO2, 0, "iovdd"},
            [AVDD] = {NULL, AXP2101_ID_BLDO2, 0, "avdd"},
            [DVDD] = {NULL, AXP2101_ID_DLDO2, 0, "dvdd"},
            [AFVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [FLVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
            [CAMERAVDD] = {NULL, AXP2101_ID_MAX, 0, ""},
        },
    }
#else
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
#elif defined CONFIG_VIN_SENSOR_gc2053
            [0] = 
                {
                    .cam_name = "gc2053_mipi",
                    .cam_addr = 0x6e,
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
#endif	/*CONFIG_DRIVER_SYSCONFIG*/
};
#endif
