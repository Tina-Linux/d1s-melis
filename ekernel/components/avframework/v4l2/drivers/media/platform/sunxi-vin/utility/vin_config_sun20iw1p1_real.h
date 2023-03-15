#ifndef __CONFIG_SL_SUN20IW1P1_H_
#define __CONFIG_SL_SUN20IW1P1_H_

#include <sunxi_hal_regulator.h>
#include <hal_gpio.h>
#include "../vin.h"
#include "../vin-video/vin_core.h"
#include <hal_reset.h>

unsigned int vin_csi_base[VIN_MAX_CSI] = {
	CSI0_REGS_BASE,
};

int vin_isp_base[VIN_MAX_ISP] = {
};

unsigned int vin_isp_irq[VIN_MAX_ISP] = {
};

unsigned int vin_mipi_base[VIN_MAX_MIPI] = {
};

unsigned int vin_scaler_base[VIN_MAX_SCALER] = {
};

unsigned int vin_dma_base[VIN_MAX_DEV] = {
	CSI_DMA0_REG_BASE,
	CSI_DMA1_REG_BASE,
};

int vin_dma_irq[VIN_MAX_DEV] = {
	SUNXI_IRQ_CSIC_DMA0,
	SUNXI_IRQ_CSIC_DMA1,
};

struct vin_clk_info vind_default_clk[VIN_MAX_CLK] = {
	[VIN_TOP_CLK] = {
		.clock = CLK_CSI_TOP,
#ifndef CONFIG_DRIVER_SYSCONFIG
		.frequency = 336000000,
#endif
	},
	[VIN_TOP_CLK_SRC] = {
		.clock = CLK_PLL_VIDEO1_2X,
	},
};

struct vin_clk_info vind_default_bus_clk[VIN_MAX_BUS_CLK] = {
	[VIN_CSI_BUS_CLK] = {
		.clock = CLK_BUS_CSI,
	},
	[VIN_CSI_MBUS_CLK] = {
		.clock = CLK_MBUS_CSI,
	},
};

struct reset_control *bus_rst = RST_BUS_CSI;

/* watchout: CLK_SRC_HOSC24M type is HAL_SUNXI_FIXED_CCU */
struct vin_mclk_info vind_default_mclk[VIN_MAX_CCI] = {
	{
		.mclk = CLK_CSI0_MCLK,
#if defined CONFIG_CSI_PLL_CLK_SPREAD_SPECTRUM
		.clk_24m = CLK_SRC_HOSC24M,
#else
		.clk_24m = CLK_SRC_HOSC24M,
#endif
		.clk_pll = CLK_PLL_VIDEO1,
		.pin = {
			[0] = {0, GPIOE(3), 2, 0, 2},
			[1] = {0, GPIOE(3), 15, 0, 2},
		},
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
	"csi1_d4", "csi1_d5", "csi1_d6", "csi1_d7";
*/
int vind_csi_parallel_pins[VIN_MAX_CSI][11] = {
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
#endif /*__CONFIG_SL_SUN20IW1P1_H_*/
