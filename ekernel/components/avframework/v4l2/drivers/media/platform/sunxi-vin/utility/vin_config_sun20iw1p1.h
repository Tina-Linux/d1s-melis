#ifndef __CONFIG_SL_SUN20IW1P1_H_
#define __CONFIG_SL_SUN20IW1P1_H_

#include <sunxi_hal_regulator.h>
#include <hal_gpio.h>
#include "../vin.h"
#include "../vin-video/vin_core.h"

extern unsigned int vin_csi_base[VIN_MAX_CSI];

extern unsigned int vin_isp_base[VIN_MAX_ISP];

extern int vin_isp_irq[VIN_MAX_ISP];

extern unsigned int vin_mipi_base[VIN_MAX_MIPI];

extern unsigned int vin_scaler_base[VIN_MAX_SCALER];

extern unsigned int vin_dma_base[VIN_MAX_DEV];

extern int vin_dma_irq[VIN_MAX_DEV];

extern struct vin_clk_info vind_default_clk[VIN_MAX_CLK];

extern struct reset_control *bus_rst;

extern struct vin_clk_info vind_default_bus_clk[VIN_MAX_BUS_CLK];

/* watchout: CLK_SRC_HOSC24M type is HAL_SUNXI_FIXED_CCU */
extern struct vin_mclk_info vind_default_mclk[VIN_MAX_CCI];

extern struct vin_clk_info vind_default_isp_clk[VIN_ISP_MAX_CLK];

extern struct vin_core sunxi_vinc[VIN_MAX_DEV];

extern struct sensor_list sensors_default[VIN_MAX_DEV];

/*
	"csi1_pck", "csi1_hsync", "csi1_vsync",
	"csi1_d0", "csi1_d1", "csi1_d2", "csi1_d3",
	"csi1_d4", "csi1_d5", "csi1_d6", "csi1_d7";
*/
extern int vind_csi_parallel_pins[VIN_MAX_CSI][11];

#endif /*__CONFIG_SL_SUN20IW1P1_H_*/
