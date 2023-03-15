#include <sunxi_hal_common.h>
#include "vin_config_sun8iw21p1.h"

unsigned long syscfg_base = SYSCFG_REGS_BASE;
unsigned long rtc_base = RTC_REGS_BASE;

unsigned long vin_csi_base[VIN_MAX_CSI] = {
	CSI0_REGS_BASE,
	CSI1_REGS_BASE,
	CSI2_REGS_BASE,
};

unsigned long vin_tdm_base[VIN_MAX_TDM] = {
	TDM_REGS_BASE,
};

unsigned long vin_isp_base[VIN_MAX_ISP/DEV_VIRT_NUM] = {
	ISP_REGS_BASE,
};

unsigned long vin_mipi_base[1 + 2*VIN_MAX_MIPI] = {
	MIPI_REGS_BASE,
	MIPI_PHYA_REGS_BASE,
	MIPI_PHYB_REGS_BASE,
	MIPI_PORT0_REGS_BASE,
	MIPI_PORT1_REGS_BASE,
};

unsigned long vin_scaler_base[VIN_MAX_SCALER/DEV_VIRT_NUM] = {
	VIPP0_REGS_BASE,
	VIPP1_REGS_BASE,
	VIPP2_REGS_BASE,
	VIPP3_REGS_BASE,
};

unsigned int vin_tdm_irq[VIN_MAX_TDM] = {
	SUNXI_IRQ_TDM,
};

unsigned int vin_isp_irq[VIN_MAX_ISP] = {
	SUNXI_IRQ_ISP0,
	SUNXI_IRQ_ISP1,
	SUNXI_IRQ_ISP2,
	SUNXI_IRQ_ISP3,
};

unsigned int vin_vipp_irq[VIN_MAX_SCALER/DEV_VIRT_NUM] = {
	SUNXI_IRQ_VIPP0,
	SUNXI_IRQ_VIPP1,
	SUNXI_IRQ_VIPP2,
	SUNXI_IRQ_VIPP3,
};

struct vin_clk_info vind_default_clk[VIN_MAX_CLK] = {
	[VIN_TOP_CLK] = {
		.clock = HAL_CLK_PERIPH_CSI_TOP,
		.frequency = 300000000,
	},
	[VIN_TOP_CLK_SRC] = {
		.clock = HAL_CLK_PLL_CSIX4,
	},
	[VIN_TOP_CLK_SRC1] = {
		.clock = HAL_CLK_PLL_PERI0300M,
		.frequency = 300000000,
	},
};

struct vin_clk_info vind_default_isp_clk[VIN_ISP_MAX_CLK] = {
	[VIN_ISP_CLK] = {
		.clock = HAL_CLK_PERIPH_ISP,
	},
};

struct vin_mipi_gpio_info vin_mipi_gpio[VIN_MAX_MIPI] = {
	{
		.pin = {
			GPIOA(0), GPIOA(1), GPIOA(2), GPIOA(3), GPIOA(4), GPIOA(5),
			},
		.pin_func = {0x2, 0xf},
	},
	{
		.pin = {
			GPIOA(6), GPIOA(7), GPIOA(8), GPIOA(9), GPIOA(10), GPIOA(11),
			},
		.pin_func = {0x2, 0xf},
	},
};

struct sensor_list global_sensors_list[2][MAX_DETECT_SENSOR] = {
	[0]= {
		[0] = {
			.sensor_name = "gc2053_mipi",
			.sensor_twi_addr = 0x6e,
			.sensor_twi_id = 1,
			.mclk_id = 0,
			.use_isp = 1,
			.id = 0,
			.addr_width = 8,
			.data_width = 8,
			.reset_gpio = GPIOA(11),
			.pwdn_gpio = GPIOA(9),
			.ir_cut_gpio[0] = GPIOD(18), /*-cut*/
			.ir_cut_gpio[1] = GPIOD(8),  /*+cut*/
			.ir_led_gpio = 0xffff, //GPIOE(10)
		},
		[1] = {
			.sensor_name = "gc4663_mipi",
			.sensor_twi_addr = 0x52,
			.sensor_twi_id = 1,
			.mclk_id = 0,
			.use_isp = 1,
			.id = 1,
			.addr_width = 16,
			.data_width = 8,
			.reset_gpio = GPIOA(11),
			.pwdn_gpio = GPIOA(9),
			.ir_cut_gpio[0] = GPIOD(18), /*-cut*/
			.ir_cut_gpio[1] = GPIOD(8),  /*+cut*/
			.ir_led_gpio = 0xffff, //GPIOE(10)
		},
	},
	[1]= {
		[0] = {
			.sensor_name = "gc2053_mipi",
			.sensor_twi_addr = 0x6e,
			.sensor_twi_id = 0,
			.mclk_id = 1,
			.use_isp = 1,
			.id = 0,
			.addr_width = 8,
			.data_width = 8,
			.reset_gpio = GPIOE(8),
			.pwdn_gpio = GPIOE(9),
			.ir_cut_gpio[0] = 0xffff,/*-cut*/
			.ir_cut_gpio[1] = 0xffff,/*+cut*/
			.ir_led_gpio = 0xffff,
		},
		[1] = {
			.sensor_name = "gc4663_mipi",
			.sensor_twi_addr = 0x52,
			.sensor_twi_id = 0,
			.mclk_id = 1,
			.use_isp = 1,
			.id = 1,
			.addr_width = 16,
			.data_width = 8,
			.reset_gpio = GPIOE(8),
			.pwdn_gpio = GPIOE(9),
			.ir_cut_gpio[0] = 0xffff,/*-cut*/
			.ir_cut_gpio[1] = 0xffff,/*+cut*/
			.ir_led_gpio = 0xffff,
		},
	},
};

#if (CONFIG_ISP_NUMBER == 1)
struct vin_mclk_info vind_default_mclk[VIN_MAX_CCI] = {
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER0,
		.clk_24m = HAL_CLK_SRC_HOSC,
		.clk_pll = HAL_CLK_PLL_CSI,
		.pin = GPIOA(10),
		.pin_func = {0x4, 0xf},
	},
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER1,
		.clk_24m = HAL_CLK_SRC_HOSC,
		.clk_pll = HAL_CLK_PLL_CSI,
		.pin = GPIOA(13),
		.pin_func = {0x4, 0xf},
	},
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER1,
		.clk_24m = HAL_CLK_SRC_HOSC,
		.clk_pll = HAL_CLK_PLL_CSI,
		.pin = GPIOE(1),
		.pin_func = {0x2, 0xf},
	},
};

struct sensor_list global_sensors[VIN_MAX_CSI] = {
	/*mipi0 parser0*/
	[0] = {
#ifdef CONFIG_SENSOR_GC4663_MIPI
		.used = 1,
		.sensor_name = "gc4663_mipi",
		.sensor_twi_addr = 0x52,
		.sensor_twi_id = 1,
		.mclk_id = 0,
		.use_isp = 1,
		.id = 0,
		.addr_width = 16,
		.data_width = 8,
		.reset_gpio = GPIOA(11),
		.pwdn_gpio = GPIOA(9),
		.ir_cut_gpio[0] = GPIOD(18), /*-cut*/
		.ir_cut_gpio[1] = GPIOD(8),  /*+cut*/
		.ir_led_gpio = 0xffff, //GPIOE(10)
#else //CONFIG_SENSOR_GC2053_MIPI
		.used = 1,
		.sensor_name = "gc2053_mipi",
		.sensor_twi_addr = 0x6e,
		.sensor_twi_id = 1,
		.mclk_id = 0,
		.use_isp = 1,
		.id = 0,
		.addr_width = 8,
		.data_width = 8,
		.reset_gpio = GPIOA(11),
		.pwdn_gpio = GPIOA(9),
		.ir_cut_gpio[0] = GPIOD(18), /*-cut*/
		.ir_cut_gpio[1] = GPIOD(8),  /*+cut*/
		.ir_led_gpio = 0xffff, //GPIOE(10)
#endif
	},
	/*mipi1 parser1*/
	[1] = {
		.used = 0,
		.sensor_name = "gc4663_mipi",
		.sensor_twi_addr = 0x52,
		.sensor_twi_id = 0,
		.mclk_id = 1,
		.use_isp = 1,
		.id = 1,
		.addr_width = 16,
		.data_width = 8,
		.reset_gpio = GPIOA(20),
		.pwdn_gpio = GPIOA(21),
		.ir_cut_gpio[0] = 0xffff,/*-cut*/
		.ir_cut_gpio[1] = 0xffff,/*+cut*/
		.ir_led_gpio = 0xffff,
	},
};

struct vin_core global_video[VIN_MAX_VIDEO] = {
	[0] = {
		.used = 1,
		.id = 0,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 0,
		.mipi_sel = 0,
		.isp_sel = 0,
		.tdm_rx_sel = 0xff,
		.isp_tx_ch = 0,
		.base = CSI_DMA0_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA0,
		.o_width = 1280,
		.o_height = 720,
		//.fourcc = V4L2_PIX_FMT_LBC_2_5X,
		.fourcc = V4L2_PIX_FMT_NV12,
		.use_sensor_list = 0,
	},
#if 0
	[4] = {
		.used = 0,
		.id = 4,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 0,
		.mipi_sel = 0,
		.isp_sel = 0,
		.tdm_rx_sel = 0xff,
		.isp_tx_ch = 0,
		.base = CSI_DMA1_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA1,
		.o_width = 640,
		.o_height = 480,
		.fourcc = V4L2_PIX_FMT_LBC_2_5X,
	},
	[8] = {
		.used = 0,
		.id = 8,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 0,
		.mipi_sel = 0,
		.isp_sel = 0,
		.tdm_rx_sel = 0xff,
		.isp_tx_ch = 0,
		.base = CSI_DMA2_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA2,
		.o_width = 640,
		.o_height = 480,
		.fourcc = V4L2_PIX_FMT_NV12,
		},
	[12]= {
		.used = 0,
		.id = 12,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 0,
		.mipi_sel = 0,
		.isp_sel = 0,
		.tdm_rx_sel = 0xff,
		.isp_tx_ch = 0,
		.base = CSI_DMA3_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA3,
		.o_width = 1920,
		.o_height = 1080,
		.fourcc = V4L2_PIX_FMT_NV12,
	}
#endif
};
bool vin_work_mode = 0;
#else //CONFIG_ISP_NUMBER == 2
struct vin_mclk_info vind_default_mclk[VIN_MAX_CCI] = {
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER0,
		.clk_24m = HAL_CLK_SRC_HOSC,
		.clk_pll = HAL_CLK_PLL_CSI,
		.pin = GPIOE(12),
		.pin_func = {0x5, 0xf},
	},
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER1,
		.clk_24m = HAL_CLK_SRC_HOSC,
		.clk_pll = HAL_CLK_PLL_CSI,
		.pin = GPIOE(13),
		.pin_func = {0x5, 0xf},
	},
	{
		.mclk = HAL_CLK_PERIPH_CSI_MASTER1,
		.clk_24m = HAL_CLK_SRC_HOSC,
		.clk_pll = HAL_CLK_PLL_CSI,
		.pin = GPIOE(1),
		.pin_func = {0x2, 0xf},
	},
};

struct sensor_list global_sensors[VIN_MAX_CSI] = {
	/*mipi0 parser0*/
	[0] = {
		.used = 1,
		.sensor_name = "gc2053_mipi",
		.sensor_twi_addr = 0x6e,
		.sensor_twi_id = 1,
		.mclk_id = 0,
		.use_isp = 1,
		.id = 0,
		.addr_width = 8,
		.data_width = 8,
		.reset_gpio = GPIOE(6),
		.pwdn_gpio = GPIOE(7),
		.ir_cut_gpio[0] = 0xffff,/*-cut*/
		.ir_cut_gpio[1] = 0xffff,/*+cut*/
		.ir_led_gpio = 0xffff,
	},
	/*mipi1 parser1*/
	[1] = {
		.used = 1,
		.sensor_name = "gc2053_mipi",
		.sensor_twi_addr = 0x6e,
		.sensor_twi_id = 0,
		.mclk_id = 1,
		.use_isp = 1,
		.id = 1,
		.addr_width = 8,
		.data_width = 8,
		.reset_gpio = GPIOE(8),
		.pwdn_gpio = GPIOE(9),
		.ir_cut_gpio[0] = 0xffff,/*-cut*/
		.ir_cut_gpio[1] = 0xffff,/*+cut*/
		.ir_led_gpio = 0xffff,
	},
};

struct vin_core global_video[VIN_MAX_VIDEO] = {
	[0] = {
		.used = 1,
		.id = 0,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 0,
		.mipi_sel = 0,
		.isp_sel = 0,
		.tdm_rx_sel = 0,
		.isp_tx_ch = 0,
		.base = CSI_DMA0_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA0,
		.o_width = 512,
		.o_height = 288,
		//.fourcc = V4L2_PIX_FMT_LBC_2_5X,
		.fourcc = V4L2_PIX_FMT_NV12,
		.use_sensor_list = 0,
	},
	[1] = {
		.used = 1,
		.id = 1,
		.rear_sensor = 0,
		.front_sensor = 0,
		.csi_sel = 1,
		.mipi_sel = 1,
		.isp_sel = 1,
		.tdm_rx_sel = 1,
		.isp_tx_ch = 0,
		.base = CSI_DMA0_REG_BASE,
		.irq = SUNXI_IRQ_CSIC_DMA0,
		.o_width = 512,
		.o_height = 288,
		//.fourcc = V4L2_PIX_FMT_LBC_2_5X,
		.fourcc = V4L2_PIX_FMT_NV12,
		.use_sensor_list = 0,
	},
};
bool vin_work_mode = 1;
#endif
