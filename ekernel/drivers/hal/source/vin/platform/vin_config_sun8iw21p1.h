#ifndef __CONFIG_VIN_SUN8IW21P1_H_
#define __CONFIG_VIN_SUN8IW21P1_H_

#include <hal_gpio.h>
#include <hal_clk.h>

#include "../vin_video/vin_core.h"

#define SYSCFG_REGS_BASE			0x03000000
#define RTC_REGS_BASE				0x07090000

#define CSI_CCU_REGS_BASE			0x05800000
#define CSI_TOP_REGS_BASE			0x05800800

#define CSI0_REGS_BASE				0x05820000
#define CSI1_REGS_BASE				0x05821000
#define CSI2_REGS_BASE				0x05822000

#define MIPI_REGS_BASE				0x05810000
#define MIPI_PHYA_REGS_BASE			0x05810100
#define MIPI_PHYB_REGS_BASE			0x05810200
#define MIPI_PORT0_REGS_BASE			0x05811000
#define MIPI_PORT1_REGS_BASE			0x05811400

#define TDM_REGS_BASE				0x05908000

#define ISP_REGS_BASE				0x05900000

#define VIPP0_REGS_BASE				0x05910000
#define VIPP1_REGS_BASE				0x05910400
#define VIPP2_REGS_BASE				0x05910800
#define VIPP3_REGS_BASE				0x05910c00

#define CSI_DMA0_REG_BASE			0x05830000
#define CSI_DMA1_REG_BASE			0x05831000
#define CSI_DMA2_REG_BASE			0x05832000
#define CSI_DMA3_REG_BASE			0x05833000

#define SUNXIGIC_START 				16
#define SUNXI_IRQ_CSIC_DMA0			(127 - SUNXIGIC_START)
#define SUNXI_IRQ_CSIC_DMA1			(128 - SUNXIGIC_START)
#define SUNXI_IRQ_CSIC_DMA2			(129 - SUNXIGIC_START)
#define SUNXI_IRQ_CSIC_DMA3			(130 - SUNXIGIC_START)
#define SUNXI_IRQ_TDM 				(137 - SUNXIGIC_START)
#define SUNXI_IRQ_CSI_TOP_PKT 			(138 - SUNXIGIC_START)
#define SUNXI_IRQ_ISP0 				(140 - SUNXIGIC_START)
#define SUNXI_IRQ_ISP1 				(141 - SUNXIGIC_START)
#define SUNXI_IRQ_ISP2 				(142 - SUNXIGIC_START)
#define SUNXI_IRQ_ISP3 				(143 - SUNXIGIC_START)
#define SUNXI_IRQ_VIPP0 			(144 - SUNXIGIC_START)
#define SUNXI_IRQ_VIPP1 			(145 - SUNXIGIC_START)
#define SUNXI_IRQ_VIPP2 			(146 - SUNXIGIC_START)
#define SUNXI_IRQ_VIPP3 			(147 - SUNXIGIC_START)

#define DEV_VIRT_NUM			4

#define VIN_MAX_DEV			16
#define VIN_MAX_CSI			3
#define VIN_MAX_CCI			3
#define VIN_MAX_TDM			1
#define VIN_MAX_MIPI			2
#define VIN_MAX_ISP			5
#define VIN_MAX_SCALER			16
#define VIN_MAX_VIDEO			16
#define MAX_CH_NUM			4

#define MAX_DETECT_SENSOR		3

enum {
	VIN_TOP_CLK = 0,
	VIN_TOP_CLK_SRC,
	VIN_TOP_CLK_SRC1,
	VIN_MAX_CLK,
};

enum {
	VIN_ISP_CLK = 0,
	VIN_ISP_CLK_SRC,
	VIN_ISP_MAX_CLK,
};

struct vin_clk_info {
	hal_clk_t clock;
	unsigned long frequency;
};

struct vin_mclk_info {
	hal_clk_t mclk;
	hal_clk_t clk_24m;
	hal_clk_t clk_pll;
	gpio_pin_t pin;
	unsigned char pin_func[2];
	unsigned long frequency;
	unsigned int use_count;
};

struct vin_mipi_gpio_info {
	gpio_pin_t pin[6];
	unsigned char pin_func[2];
};

struct sensor_list {
	char sensor_name[16];
	int used;
	int sensor_twi_addr;
	int sensor_twi_id;
	int mclk_id;
	int use_isp;
	int id;
	int addr_width;
	int data_width;
	gpio_pin_t reset_gpio;
	gpio_pin_t pwdn_gpio;
	gpio_pin_t ir_cut_gpio[2];
	gpio_pin_t ir_led_gpio;
	struct sensor_fuc_core *sensor_core;
};

extern struct vin_clk_info vind_default_clk[VIN_MAX_CLK];
extern struct vin_mclk_info vind_default_mclk[VIN_MAX_CCI];
extern struct vin_clk_info vind_default_isp_clk[VIN_ISP_MAX_CLK];
extern struct vin_mipi_gpio_info vin_mipi_gpio[VIN_MAX_MIPI];

extern unsigned long syscfg_base;
extern unsigned long rtc_base;
extern unsigned long vin_csi_base[VIN_MAX_CSI];
extern unsigned long vin_tdm_base[VIN_MAX_TDM];
extern unsigned long vin_isp_base[VIN_MAX_ISP/DEV_VIRT_NUM];
extern unsigned long vin_mipi_base[1 + 2*VIN_MAX_MIPI];
extern unsigned long vin_scaler_base[VIN_MAX_SCALER/DEV_VIRT_NUM];

extern unsigned int vin_tdm_irq[VIN_MAX_TDM];
extern unsigned int vin_isp_irq[VIN_MAX_ISP];
extern unsigned int vin_vipp_irq[VIN_MAX_SCALER/DEV_VIRT_NUM];

extern struct vin_core global_video[VIN_MAX_VIDEO];
extern struct sensor_list global_sensors[VIN_MAX_CSI];
extern bool vin_work_mode;

extern struct sensor_list global_sensors_list[2][MAX_DETECT_SENSOR];

#endif /*__CONFIG_VIN_SUN8IW21P1_H_*/

