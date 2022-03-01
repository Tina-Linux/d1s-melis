#ifndef __CONFIG_SL_SUN8IW19P1_H_
#define __CONFIG_SL_SUN8IW19P1_H_

#include <sunxi_hal_regulator.h>
#include <hal_gpio.h>
#include "../vin.h"
#include "../vin-video/vin_core.h"

extern unsigned int vin_csi_base[VIN_MAX_CSI];

extern unsigned int vin_isp_base[VIN_MAX_ISP];
extern unsigned int vin_isp_irq[VIN_MAX_ISP];
extern unsigned int vin_mipi_base[VIN_MAX_MIPI];
extern unsigned int vin_scaler_base[VIN_MAX_SCALER];
extern struct vin_clk_info vind_default_clk[VIN_MAX_CLK];
extern struct vin_mclk_info vind_default_mclk[VIN_MAX_CCI];
extern struct vin_clk_info vind_default_isp_clk[VIN_ISP_MAX_CLK];
extern struct vin_core sunxi_vinc[VIN_MAX_DEV];
extern struct sensor_list sensors_default[VIN_MAX_DEV];
#endif
