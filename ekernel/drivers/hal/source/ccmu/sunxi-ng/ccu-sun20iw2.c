// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 huangzhenwei@allwinnertech.com
 */
#include "ccu.h"
#include "ccu_common.h"
#include "ccu_reset.h"
#include "ccu_div.h"
#include "ccu_gate.h"
#include "ccu_mp.h"
#include "ccu_mult.h"
#include "ccu_nk.h"
#include "ccu_nkm.h"
#include "ccu_nkmp.h"
#include "ccu_nm.h"

#include "ccu-sun20iw2.h"
#include "ccu-sun20iw2-aon.h"
#include <hal_clk.h>
#include <hal_reset.h>

/* ccu_des_start */

/* BUS Clock Gating Control Register0 */
static SUNXI_CCU_GATE(bus_ehci0_clk, "bus-ohci0", "osc12M", 0x004, BIT(31), 0);
static SUNXI_CCU_GATE(bus_ohci0_clk, "bus-ohci0", "osc12M", 0x004, BIT(30), 0);
static SUNXI_CCU_GATE(bus_csi_jpe_clk, "bus-csi-jpe", "osc12M", 0x004, BIT(29), 0);
static SUNXI_CCU_GATE(bus_ledc_clk, "bus-ledc", "hosc",0x004, BIT(28), 0);
static SUNXI_CCU_GATE(bus_otg_clk, "bus-otg", "hosc", 0x004, BIT(27), 0);
static SUNXI_CCU_GATE(bus_smcard_clk, "bus-smcard", "hosc", 0x004, BIT(26), 0);

static SUNXI_CCU_GATE(bus_hspsram_ctrl_clk, "bus-hspsram-ctrl", "hosc", 0x004, BIT(21), 0);

static SUNXI_CCU_GATE(ir_rx_clk, "ir_rx", "hosc", 0x004, BIT(16), 0);
static SUNXI_CCU_GATE(ir_tx_clk, "ir_tx", "hosc", 0x004, BIT(15), 0);
static SUNXI_CCU_GATE(bus_pwm_clk, "bus-pwm", "apb", 0x004, BIT(14), 0);
static SUNXI_CCU_GATE(bus_twi1_clk, "bus-twi1", "apb", 0x004, BIT(11), 0);
static SUNXI_CCU_GATE(bus_twi0_clk, "bus-twi0", "apb", 0x004, BIT(10), 0);

static SUNXI_CCU_GATE(bus_uart2_clk, "bus-uart2", "apb", 0x004, BIT(8), 0);
static SUNXI_CCU_GATE(bus_uart1_clk, "bus-uart1", "apb", 0x004, BIT(7), 0);
static SUNXI_CCU_GATE(bus_uart0_clk, "bus-uart0", "apb", 0x004, BIT(6), 0);

static SUNXI_CCU_GATE(bus_sdc0_clk, "bus-sdc0", "apb", 0x004, BIT(4), 0);

static SUNXI_CCU_GATE(bus_spi1_clk, "bus-spi1", "osc12M", 0x004, BIT(1), 0);
static SUNXI_CCU_GATE(bus_spi0_clk, "bus-spi0", "osc12M", 0x004, BIT(0), 0);

/* BUS Clock Gating Control Register1 */
static SUNXI_CCU_GATE(bus_monitor_clk, "bus-monitor", "osc12M", 0x008, BIT(28), 0);
static SUNXI_CCU_GATE(bus_g2d_clk, "bus-g2d", "osc12M", 0x008, BIT(27), 0);
static SUNXI_CCU_GATE(bus_de_clk, "bus-de", "osc12M", 0x008, BIT(26), 0);
static SUNXI_CCU_GATE(bus_display_clk, "bus-display", "osc12M", 0x008, BIT(25), 0);
static SUNXI_CCU_GATE(bus_lcd_clk, "bus-lcd", "osc12M", 0x008, BIT(24), 0);

static SUNXI_CCU_GATE(bus_bt_core_clk, "bus-bt-core", "osc12M", 0x008, BIT(21), 0);
static SUNXI_CCU_GATE(bus_wlan_ctrl_clk, "bus-wlan-ctrl", "osc12M", 0x008, BIT(20), 0);

static SUNXI_CCU_GATE(bus_trng_clk, "bus-trng", "osc12M", 0x008, BIT(14), 0);
static SUNXI_CCU_GATE(bus_spc_clk, "bus-spc", "osc12M", 0x008, BIT(13), 0);
static SUNXI_CCU_GATE(bus_ss_clk, "bus-ss", "osc12M", 0x008, BIT(12), 0);
static SUNXI_CCU_GATE(bus_timer_clk, "bus-timer", "osc12M", 0x008, BIT(11), 0);
static SUNXI_CCU_GATE(bus_spinlock_clk, "bus-spinlock", "hosc", 0x008, BIT(10), 0);

static SUNXI_CCU_GATE(bus_dma1_clk, "bus-dma1", "hosc", 0x008, BIT(7), 0);
static SUNXI_CCU_GATE(bus_dma0_clk, "bus-dma0", "hosc", 0x008, BIT(6), 0);

static SUNXI_CCU_GATE(bus_spdif_clk, "bus-spdif", "apb", 0x008, BIT(2), 0);
static SUNXI_CCU_GATE(bus_i2s_clk, "bus-i2s", "hosc", 0x008, BIT(1), 0);

/* CPU_DSP_RV Systems Clock Gating Control Register */
static SUNXI_CCU_GATE(riscv_cfg_clk, "riscv-cfg", "hosc", 0x014, BIT(19), 0);
static SUNXI_CCU_GATE(riscv_msgbox_clk, "riscv-msgbox", "hosc", 0x014, BIT(18), 0);

static SUNXI_CCU_GATE(dsp_cfg_clk, "dsp-cfg", "hosc", 0x014, BIT(11), 0);
static SUNXI_CCU_GATE(dsp_msgbox_clk, "dsp-msgbox", "hosc", 0x014, BIT(10), 0);
static SUNXI_CCU_GATE(cpu_msgbox_clk, "cpu-msgbox", "hosc", 0x014, BIT(1), 0);

/* rst_def_start */
static struct ccu_reset_map sun20iw2_ccu_resets[] =
{
/* Module Reset Control Register0 */
    [RST_USB_EHCI]      = { 0x00c, BIT(31) },
    [RST_USB_OHCI]      = { 0x00c, BIT(30) },
    [RST_CSI_JPE]       = { 0x00c, BIT(29) },
    [RST_LEDC]          = { 0x00c, BIT(28) },
    [RST_USB_OTG]       = { 0x00c, BIT(27) },
    [RST_SMCARD]        = { 0x00c, BIT(26) },
    [RST_USB_PHY]       = { 0x00c, BIT(25) },
    [RST_FLASH_ENC]     = { 0x00c, BIT(23) },
    [RST_FLASH_CTRL]    = { 0x00c, BIT(22) },
    [RST_HSPSRAM_CTRL]  = { 0x00c, BIT(21) },
    [RST_LSPSRAM_CTRL]  = { 0x00c, BIT(20) },
    [RST_IRRX]		= { 0x00c, BIT(16) },
    [RST_IRTX]          = { 0x00c, BIT(15) },
    [RST_PWM]		= { 0x00c, BIT(14) },
    [RST_TWI1]          = { 0x00c, BIT(11) },
    [RST_TWI0]          = { 0x00c, BIT(10) },
    [RST_UART2]         = { 0x00c, BIT(8) },
    [RST_UART1]         = { 0x00c, BIT(7) },
    [RST_UART0]         = { 0x00c, BIT(6) },
    [RST_SDC0]          = { 0x00c, BIT(4) },
    [RST_SPI1]          = { 0x00c, BIT(1) },
    [RST_SPI0]          = { 0x00c, BIT(0) },

/* Module Reset Control Register0 */
    [RST_G2D]           = { 0x010, BIT(27) },
    [RST_DE]            = { 0x010, BIT(26) },
    [RST_DISPLAY]       = { 0x010, BIT(25) },
    [RST_LCD]           = { 0x010, BIT(24) },
    [RST_BT_CORE]       = { 0x010, BIT(21) },
    [RST_WLAN_CTRL]     = { 0x010, BIT(20) },
    [RST_TRNG]          = { 0x010, BIT(14) },
    [RST_SPC]           = { 0x010, BIT(13) },
    [RST_SS]		= { 0x010, BIT(12) },
    [RST_TIMER]         = { 0x010, BIT(11) },
    [RST_SPINLOCK]      = { 0x010, BIT(10) },
    [RST_DMA1]		= { 0x010, BIT(7) },
    [RST_DMA0]		= { 0x010, BIT(6) },
    [RST_SPDIF]		= { 0x010, BIT(2) },
    [RST_I2S]		= { 0x010, BIT(1) },

/* CPU_DSP_RV Systems Reset Control Register */
    [RST_RISCV_SYS_APB_SOFT]   = { 0x018, BIT(21) },
    [RST_RISCV_TIMESTAMP]      = { 0x018, BIT(20) },
    [RST_RISCV_CFG]        = { 0x018, BIT(19) },
    [RST_RISCV_MSGBOX]         = { 0x018, BIT(18) },
    [RST_RISCV_WDG]            = { 0x018, BIT(17) },
    [RST_RISCV_CORE]           = { 0x018, BIT(16) },

    [RST_DSP_DEBUG]      = { 0x018, BIT(14) },
    [RST_DSP_INTC]       = { 0x018, BIT(13) },
    [RST_DSP_TZMA]       = { 0x018, BIT(12) },
    [RST_DSP_CFG]        = { 0x018, BIT(11) },
    [RST_DSP_MSGBOX]     = { 0x018, BIT(10) },
    [RST_DSP_WDG]        = { 0x018, BIT(9) },
    [RST_DSP_CORE]       = { 0x018, BIT(8) },

    [RST_CPU_CFG]        = { 0x018, BIT(2) },
    [RST_CPU_MSGBOX]     = { 0x018, BIT(1) },
    [RST_CPU_WDG]        = { 0x018, BIT(0) },
};
/* rst_def_end */

/* MBUS Clock Gating Control Register */
static SUNXI_CCU_GATE(mbus_de_clk, "mbus-de", "hosc", 0x01C, BIT(9), 0);
static SUNXI_CCU_GATE(mbus_g2d_clk, "mbus-g2d", "hosc", 0x01C, BIT(8), 0);
static SUNXI_CCU_GATE(mbus_csi_clk, "mbus-csi", "hosc", 0x01C, BIT(7), 0);
static SUNXI_CCU_GATE(mbus_dma1_clk, "mbus-dma1", "hosc", 0x01C, BIT(6), 0);
static SUNXI_CCU_GATE(mbus_dma0_clk, "mbus-dma0", "hosc", 0x01C, BIT(5), 0);
static SUNXI_CCU_GATE(mbus_usb_clk, "mbus-usb", "hosc", 0x01C, BIT(4), 0);
static SUNXI_CCU_GATE(mbus_ce_clk, "mbus-ce", "hosc", 0x01C, BIT(3), 0);
static SUNXI_CCU_GATE(mbus_dsp_clk, "mbus-dsp", "hosc", 0x01C, BIT(2), 0);
static SUNXI_CCU_GATE(mbus_riscv_clk, "mbus-riscv", "hosc", 0x01C, BIT(1), 0);
static SUNXI_CCU_GATE(mbus_cpu_clk, "mbus-cpu", "hosc", 0x01C, BIT(0), 0);

/* SPI0 Clock Control Register */
static const char *const spi_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(spi0_clk, "spi0", spi_parents, 0x020,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* SPI1 Clock Control Register */
static SUNXI_CCU_MP_WITH_MUX_GATE(spi1_clk, "spi1", spi_parents, 0x024,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* SDC Clock Control Register */
static const char *const sdc_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(sdc0_clk, "sdc0", sdc_parents, 0x028,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* SS Clock Control Register */
static const char *const ss_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(ss_clk, "ss", ss_parents, 0x02c,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* CSI_JPE Device CLK Control Register */
static const char *const csi_jpe_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(csi_jpe_clk, "csi-jpe", csi_jpe_parents, 0x030,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* LEDC Clocok Control Register */
static const char *const ledc_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(ledc_clk, "ledc", ledc_parents, 0x034,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* IRRX Clock Control Register */
static const char *const ir_parents[] = { "hosc", "fix-losc" };
static SUNXI_CCU_MP_WITH_MUX_GATE(irrx_clk, "irrx", ir_parents, 0x038,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* IRTX Clock Control Register */
static SUNXI_CCU_MP_WITH_MUX_GATE(irtx_clk, "irtx", ir_parents, 0x03c,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* System Tick Reference Clock Control Register */
static const char *const systick_ref_parents[] = { "hosc", "fix-losc" };
static SUNXI_CCU_MP_WITH_MUX_GATE(systick_ref_clk, "systick-ref", systick_ref_parents, 0x040,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* System Tick Clock Calibration Register */
static SUNXI_CCU_GATE(systick_noref_clk, "systick_noref", "hosc", 0x044,BIT(25), 0);
static SUNXI_CCU_GATE(systick_skew_clk, "systick-skew", "hosc", 0x044, BIT(24), 0);

/* CSI Output MCLK Control Register */
static const char *const csi_mclk_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(csi_mclk_clk, "csi-mclk", csi_mclk_parents, 0x050,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* Flash Controller SPI Clock Register */
static const char *const flash_spi_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(flash_spi_clk, "flash-spi", flash_spi_parents, 0x054,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* APB_SPC Clock Control Register */
static const char *const pclk_spc_parents[] = { "hosc", "device-clk", "fix-losc" };
static SUNXI_CCU_MP_WITH_MUX(pclk_spc_clk, "pclk-spc", pclk_spc_parents, 0x05c,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		0);

/* USB Clock Control Register */
static const char *const usb_pll_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MUX(usb_pll_clk, "usb-pll", usb_pll_parents, 0x060, 0, 1, 0);

/* RISCV Clock Control Register */
static const char *const riscv_parents[] = { "hosc", "fix-losc", "ckpll-c906-sel", "ckpll-c906-sel" };

static SUNXI_CCU_GATE(riscv_gate_clk, "riscv-gate", "hosc", 0x064, BIT(31), 0);
static SUNXI_CCU_MUX(riscv_sel_clk, "riscv-sel",
		riscv_parents, 0x064, 4, 2, 0);
static struct clk_div_table riscv_div_table[] =
{
	{ .val = 0, .div = 1 },
	{ .val = 1, .div = 2 },
	{ .val = 2, .div = 4 },
	{ .val = 3, .div = 8 },
	{ /* Sentinel */ },
};
static SUNXI_CCU_DIV_TABLE(riscv_div_clk, "riscv-div",
		"riscv-sel", 0x064, 0, 2,
		riscv_div_table, 0);

static struct clk_div_table riscv_axi_table[] =
{
	{ .val = 1, .div = 2 },
	{ .val = 2, .div = 3 },
	{ .val = 3, .div = 4 },
	{ /* Sentinel */ },
};
static SUNXI_CCU_DIV_TABLE(riscv_axi_clk, "riscv-axi",
		"riscv-div", 0x064, 8, 2,
		riscv_axi_table, 0);

/* DSP Clock Control Register */
static const char *const dsp_parents[] = { "hosc", "fix-losc", "ckpll-hifi5-sel", "ckpll-hifi5-sel" };
static SUNXI_CCU_GATE(dsp_gate_clk, "dsp-gate", "hosc", 0x068, BIT(31), 0);

static SUNXI_CCU_MUX(dsp_sel_clk, "dsp-sel",
		dsp_parents, 0x068, 4, 2, 0);

static struct clk_div_table dsp_div_table[] =
{
	{ .val = 0, .div = 1 },
	{ .val = 1, .div = 2 },
	{ .val = 2, .div = 4 },
	{ .val = 3, .div = 8 },
	{ /* Sentinel */ },
};
static SUNXI_CCU_DIV_TABLE(dsp_div_clk, "dsp-div",
		"dsp-sel", 0x068, 0, 2,
		dsp_div_table, 0);

/* HSPSRAM Clock Control Register */
static SUNXI_CCU_GATE(hspsram_gate_clk, "hspsram-gate", "hosc", 0x06c, BIT(31), 0);

static CLK_FIXED_FACTOR_FW_NAME(hspsram_ctrl_clk, "hspsram-ctrl", "ckpll-hspsram-sel", 8, 1, CLK_SET_RATE_PARENT);

/* LSPSRAM Clock Control Register */
static SUNXI_CCU_GATE(lspsram_gate_clk, "lspsram-gate", "hosc", 0x070, BIT(31), 0);

static const char *const lspsram_sel_parents[] = { "hosc", "ckpll-lspsram-sel" };
static SUNXI_CCU_MUX(lspsram_sel_clk, "lspsram-sel", lspsram_sel_parents, 0x070, 4, 1, 0);

static struct clk_div_table lspsram_div_table[] =
{
	{ .val = 0, .div = 1 },
	{ .val = 1, .div = 2 },
	{ .val = 2, .div = 4 },
	{ .val = 3, .div = 8 },
	{ /* Sentinel */ },
};
static SUNXI_CCU_DIV_TABLE(lspsram_div_clk, "lspsram-div",
		"lspsram-sel", 0x070, 0, 2,
		lspsram_div_table, 0);
static CLK_FIXED_FACTOR_FW_NAME(lspsram_ctrl_clk, "lspsram-ctrl", "lspsram-div", 2, 1, 0);

/* G2D Clock Control Register */
static const char *const g2d_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(g2d_clk, "g2d", g2d_parents, 0x074,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* DE Clock Control Register */
static const char *const de_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(de_clk, "de", de_parents, 0x078,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

/* LCD Clock Control Register */
static const char *const lcd_parents[] = { "hosc", "device-clk" };
static SUNXI_CCU_MP_WITH_MUX_GATE(lcd_clk, "lcd", lcd_parents, 0x07c,
		0, 4,     /* M */
		16, 2,     /* N */
		24, 2,    /* mux */
		BIT(31),  /* gate */
		0);

static CLK_FIXED_FACTOR_FW_NAME(osc12M_clk, "osc12M", "hosc", 2, 1, 0);

static struct ccu_common *sun20iw2_ccu_clks[] =
{
	&bus_pwm_clk.common,
	&bus_twi1_clk.common,
	&bus_twi0_clk.common,
	&bus_uart0_clk.common,
	&bus_uart1_clk.common,
	&bus_uart2_clk.common,
	&bus_sdc0_clk.common,
	&bus_spi0_clk.common,
	&bus_spi1_clk.common,
	&bus_monitor_clk.common,
	&bus_g2d_clk.common,
	&bus_de_clk.common,
	&bus_display_clk.common,
	&bus_lcd_clk.common,
	&bus_bt_core_clk.common,
	&bus_wlan_ctrl_clk.common,
	&bus_trng_clk.common,
	&bus_spc_clk.common,
	&bus_ss_clk.common,
	&bus_timer_clk.common,
	&bus_spinlock_clk.common,
	&bus_dma1_clk.common,
	&bus_dma0_clk.common,
	&bus_spdif_clk.common,
	&bus_i2s_clk.common,
	&riscv_cfg_clk.common,
	&riscv_msgbox_clk.common,
	&dsp_cfg_clk.common,
	&dsp_msgbox_clk.common,
	&cpu_msgbox_clk.common,
	&mbus_de_clk.common,
	&mbus_g2d_clk.common,
	&mbus_csi_clk.common,
	&mbus_dma1_clk.common,
	&mbus_dma0_clk.common,
	&mbus_usb_clk.common,
	&mbus_ce_clk.common,
	&mbus_dsp_clk.common,
	&mbus_riscv_clk.common,
	&mbus_cpu_clk.common,
	&spi0_clk.common,
	&spi1_clk.common,
	&sdc0_clk.common,
	&ss_clk.common,
	&csi_jpe_clk.common,
	&ledc_clk.common,
	&irrx_clk.common,
	&irtx_clk.common,
	&systick_ref_clk.common,
	&systick_noref_clk.common,
	&systick_skew_clk.common,
	&csi_mclk_clk.common,
	&flash_spi_clk.common,
	&pclk_spc_clk.common,
	&usb_pll_clk.common,
	&riscv_gate_clk.common,
	&riscv_axi_clk.common,
	&riscv_sel_clk.common,
	&riscv_div_clk.common,
	&dsp_gate_clk.common,
	&dsp_sel_clk.common,
	&dsp_div_clk.common,
	&hspsram_gate_clk.common,
	&lspsram_gate_clk.common,
	&lspsram_sel_clk.common,
	&lspsram_div_clk.common,
	&ir_rx_clk.common,
	&ir_tx_clk.common,
	&bus_ohci0_clk.common,
	&bus_csi_jpe_clk.common,
	&bus_ehci0_clk.common,
	&bus_otg_clk.common,
	&bus_smcard_clk.common,
	&bus_hspsram_ctrl_clk.common,
	&bus_ledc_clk.common,
	&g2d_clk.common,
	&de_clk.common,
	&lcd_clk.common,
};
/* ccu_def_start */

static struct clk_hw_onecell_data sun20iw2_hw_clks =
{
	.hws    = {
		[CLK_OSC12M]        = &osc12M_clk.hw,
		[CLK_BUS_PWM]       = &bus_pwm_clk.common.hw,
		[CLK_BUS_TWI1]      = &bus_twi1_clk.common.hw,
		[CLK_BUS_TWI0]      = &bus_twi0_clk.common.hw,
		[CLK_BUS_UART0]     = &bus_uart0_clk.common.hw,
		[CLK_BUS_UART1]     = &bus_uart1_clk.common.hw,
		[CLK_BUS_UART2]     = &bus_uart2_clk.common.hw,
		[CLK_BUS_SDC0]      = &bus_sdc0_clk.common.hw,
		[CLK_BUS_SPI0]      = &bus_spi0_clk.common.hw,
		[CLK_BUS_SPI1]      = &bus_spi1_clk.common.hw,
		[CLK_BUS_MONITOR]   = &bus_monitor_clk.common.hw,
		[CLK_BUS_G2D]       = &bus_g2d_clk.common.hw,
		[CLK_BUS_DE]        = &bus_de_clk.common.hw,
		[CLK_BUS_DISPLAY]   = &bus_display_clk.common.hw,
		[CLK_BUS_LCD]       = &bus_lcd_clk.common.hw,
		[CLK_BUS_BT_CORE]      = &bus_bt_core_clk.common.hw,
		[CLK_BUS_WLAN_CTRL]      = &bus_wlan_ctrl_clk.common.hw,
		[CLK_BUS_TRNG]      = &bus_trng_clk.common.hw,
		[CLK_BUS_SPC]      = &bus_spc_clk.common.hw,
		[CLK_BUS_SS]      = &bus_ss_clk.common.hw,
		[CLK_BUS_TIMER]      = &bus_timer_clk.common.hw,
		[CLK_BUS_SPINLOCK]      = &bus_spinlock_clk.common.hw,
		[CLK_BUS_DMA1]      = &bus_dma1_clk.common.hw,
		[CLK_BUS_DMA0]      = &bus_dma0_clk.common.hw,
		[CLK_BUS_SPDIF]     = &bus_spdif_clk.common.hw,
		[CLK_BUS_I2S]      = &bus_i2s_clk.common.hw,
		[CLK_RISCV_CFG]     = &riscv_cfg_clk.common.hw,
		[CLK_RISCV_MSGBOX]       = &riscv_msgbox_clk.common.hw,
		[CLK_DSP_CFG]   = &dsp_cfg_clk.common.hw,
		[CLK_DSP_MSGBOX]    = &dsp_msgbox_clk.common.hw,
		[CLK_CPU_MSGBOX]    = &cpu_msgbox_clk.common.hw,
		[CLK_MBUS_DE]       = &mbus_de_clk.common.hw,
		[CLK_MBUS_G2D]      = &mbus_g2d_clk.common.hw,
		[CLK_MBUS_CSI]      = &mbus_csi_clk.common.hw,
		[CLK_MBUS_DMA1]     = &mbus_dma1_clk.common.hw,
		[CLK_MBUS_DMA0]     = &mbus_dma0_clk.common.hw,
		[CLK_MBUS_USB]      = &mbus_usb_clk.common.hw,
		[CLK_MBUS_CE]       = &mbus_ce_clk.common.hw,
		[CLK_MBUS_DSP]      = &mbus_dsp_clk.common.hw,
		[CLK_MBUS_RISCV]    = &mbus_riscv_clk.common.hw,
		[CLK_MBUS_CPU]      = &mbus_cpu_clk.common.hw,
		[CLK_SPI0]          = &spi0_clk.common.hw,
		[CLK_SPI1]          = &spi1_clk.common.hw,
		[CLK_SDC0]          = &sdc0_clk.common.hw,
		[CLK_SS]            = &ss_clk.common.hw,
		[CLK_CSI_JPE]       = &csi_jpe_clk.common.hw,
		[CLK_LEDC]      = &ledc_clk.common.hw,
		[CLK_IRRX]      = &irrx_clk.common.hw,
		[CLK_IRTX]      = &irtx_clk.common.hw,
		[CLK_SYSTICK_REF]   = &systick_ref_clk.common.hw,
		[CLK_SYSTICK_NOREF] = &systick_noref_clk.common.hw,
		[CLK_SYSTICK_SKEW]  = &systick_skew_clk.common.hw,
		[CLK_CSI_MCLK]  = &csi_mclk_clk.common.hw,
		[CLK_FLASH_SPI]     = &flash_spi_clk.common.hw,
		[CLK_PCLK_SPC]      = &pclk_spc_clk.common.hw,
		[CLK_USB_PLL]       = &usb_pll_clk.common.hw,
		[CLK_RISCV_GATE]     = &riscv_gate_clk.common.hw,
		[CLK_RISCV_AXI]     = &riscv_axi_clk.common.hw,
		[CLK_RISCV_SEL]     = &riscv_sel_clk.common.hw,
		[CLK_RISCV_DIV]     = &riscv_div_clk.common.hw,
		[CLK_DSP_GATE]       = &dsp_gate_clk.common.hw,
		[CLK_DSP_SEL]       = &dsp_sel_clk.common.hw,
		[CLK_DSP_DIV]       = &dsp_div_clk.common.hw,
		[CLK_HSPSRAM_GATE]       = &hspsram_gate_clk.common.hw,
		[CLK_HSPSRAM_CTRL]       = &hspsram_ctrl_clk.hw,
		[CLK_LSPSRAM_GATE]       = &lspsram_gate_clk.common.hw,
		[CLK_LSPSRAM_SEL]   = &lspsram_sel_clk.common.hw,
		[CLK_LSPSRAM_DIV]   = &lspsram_div_clk.common.hw,
		[CLK_LSPSRAM_CTRL]   = &lspsram_ctrl_clk.hw,
		[CLK_IR_RX]     = &ir_rx_clk.common.hw,
		[CLK_IR_TX]     = &ir_tx_clk.common.hw,
		[CLK_BUS_OHCI0]     = &bus_ohci0_clk.common.hw,
		[CLK_BUS_CSI_JPE]   = &bus_csi_jpe_clk.common.hw,
		[CLK_BUS_EHCI0]     = &bus_ehci0_clk.common.hw,
		[CLK_BUS_OTG]       = &bus_otg_clk.common.hw,
		[CLK_BUS_SMCARD]    = &bus_smcard_clk.common.hw,
		[CLK_BUS_HSPSRAM_CTRL]  = &bus_hspsram_ctrl_clk.common.hw,
		[CLK_BUS_LEDC]      = &bus_ledc_clk.common.hw,
		[CLK_G2D]           = &g2d_clk.common.hw,
		[CLK_DE]            = &de_clk.common.hw,
		[CLK_LCD]           = &lcd_clk.common.hw,
	},
	.num = CLK_NUMBER,
};
/* ccu_def_end */

static const struct sunxi_ccu_desc sun20iw2_ccu_desc =
{
	.ccu_clks   = sun20iw2_ccu_clks,
	.num_ccu_clks   = ARRAY_SIZE(sun20iw2_ccu_clks),

	.hw_clks    = &sun20iw2_hw_clks,
	.clk_type   = HAL_SUNXI_CCU,

	.resets     = sun20iw2_ccu_resets,
	.reset_type = HAL_SUNXI_RESET,
	.num_resets = ARRAY_SIZE(sun20iw2_ccu_resets),
};

__attribute__((weak)) int sunxi_rtc_ccu_init(void)
{
	return 0;
}

__attribute__((weak)) int sunxi_dsp_init(void)
{
	return 0;
}

int sunxi_ccu_init(void)
{
	unsigned long reg = (unsigned long)SUNXI20_CCU_BASE;
	int ret;

	ret = sunxi_ccu_aon_init();
	if (ret) {
		return ret;
	}

	ret = ccu_common_init(reg, &sun20iw2_ccu_desc);
	if (ret) {
		return ret;
	}
	sunxi_ccu_sleep_init((void *)reg, sun20iw2_ccu_clks,
			ARRAY_SIZE(sun20iw2_ccu_clks),
			NULL, 0);

	return ret;
}
