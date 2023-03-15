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
#include "type.h"

#include "ccu-sun20iw2-aon.h"
#include <hal_clk.h>
#include <hal_reset.h>

/* ccu_des_start */
/*
 * The CPU PLL is actually NP clock, with P being /1, /2 or /4. However
 * P should only be used for output frequencies lower than 288 MHz.
 *
 * For now we can just model it as a multiplier clock, and force P to /1.
 *
 * The M factor is present in the register's description, but not in the
 * frequency formula, and it's documented as "M is only used for backdoor
 * testing", so it's not modelled and then force to 0.
 */

static struct ccu_reset_map sun20iw2_ccu_on_resets[] =
{
/* Module Reset Control Register */
    [RST_BLE_RTC]        = { 0x0c8, BIT(16) },
    [RST_MADCFG]	 = { 0x0c8, BIT(15) },
    [RST_WLAN_CONN]      = { 0x0c8, BIT(13) },
    [RST_WLAN]           = { 0x0c8, BIT(12) },
    [RST_CODEC_DAC]      = { 0x0c8, BIT(10) },
    [RST_RFAS]           = { 0x0c8, BIT(9) },
    [RST_RCCAL]          = { 0x0c8, BIT(8) },
    [RST_LPSD]           = { 0x0c8, BIT(7) },
    [RST_AON_TIMER]      = { 0x0c8, BIT(6) },
    [RST_CODEC_ADC]      = { 0x0c8, BIT(5) },
    [RST_MAD]            = { 0x0c8, BIT(4) },
    [RST_DMIC]           = { 0x0c8, BIT(3) },
    [RST_GPADC]          = { 0x0c8, BIT(2) },
    [RST_LPUART1]        = { 0x0c8, BIT(1) },
    [RST_LPUART0]	 = { 0x0c8, BIT(0) },
    [RST_BLE_32M]	 = { 0x0cc, BIT(17) },
    [RST_BLE_48M]	 = { 0x0cc, BIT(16) },

};
/* rst_def_end */

/* HOSC TYPE */
static const char *const hosc_parents[] = { "dcxo26M", "dcxo40M", "dcxo24M", "dcxo32M", "dcxo24_576M" };
static SUNXI_CCU_MUX(hosc_clk, "hosc", hosc_parents, 0x084, 0, 3, 0);
/* HOSC Frequency Detect Register0 */
static SUNXI_CCU_GATE(hosc_detect_clk, "hosc-detect", "hosc", 0x080, BIT(0), 0);

/* DPLL1 Control Register */
static struct ccu_nm dpll1_clk =
{
    .enable     = BIT(31),
    .n      = _SUNXI_CCU_MULT_OFFSET(4, 8, 0),
    .m      = _SUNXI_CCU_DIV_OFFSET(0, 4, 0), /* input divider */
    .common     = {
        .reg        = 0x08c,
        .hw.init    = CLK_HW_INIT("dpll1", "hosc",
                                  &ccu_nm_ops,
                                  CLK_SET_RATE_UNGATE),
    },
};

/* DPLL2 Control Register */
static struct ccu_nm dpll2_clk =
{
    .enable     = BIT(31),
    .n      = _SUNXI_CCU_MULT_OFFSET(4, 8, 0),
    .m      = _SUNXI_CCU_DIV_OFFSET(0, 4, 0), /* input divider */
    .common     = {
        .reg        = 0x090,
        .hw.init    = CLK_HW_INIT("dpll2", "hosc",
                                  &ccu_nm_ops,
                                  CLK_SET_RATE_UNGATE),
    },
};

/* DPLL3 Control Register */
static struct ccu_nm dpll3_clk =
{
    .enable     = BIT(31),
    .n      = _SUNXI_CCU_MULT_OFFSET(4, 8, 0),
    .m      = _SUNXI_CCU_DIV_OFFSET(0, 4, 0), /* input divider */
    .common     = {
        .reg        = 0x094,
        .hw.init    = CLK_HW_INIT("dpll3", "hosc",
                                  &ccu_nm_ops,
                                  CLK_SET_RATE_UNGATE),
    },
};

CLK_FIXED_FACTOR_FW_NAME(rfip0_dpll, "rfip0-dpll", "dpll1",
                                 12, 1, 0);

CLK_FIXED_FACTOR_FW_NAME(rfip1_dpll, "rfip1-dpll", "dpll3",
                                 12, 1, 0);

/* Audio PLL Control Register */

#define SUN20IW2_PLL_AUDIO_REG 0x4004c498
static struct ccu_sdm_setting pll_audio_sdm_table[] =
{
    { .rate = 45158400, .pattern = 0xc001bcd3, .m = 18, .n = 33 },
    { .rate = 49152000, .pattern = 0xc001eb85, .m = 20, .n = 40 },
    { .rate = 90370370, .pattern = 0xc001288d, .m = 27, .n = 61 },
    { .rate = 98333333, .pattern = 0xc001eb85, .m = 24, .n = 59 },
};

static struct ccu_nm pll_audio_clk =
{
    .lock       = BIT(28),
    .n      = _SUNXI_CCU_MULT(8, 7),
    .m      = _SUNXI_CCU_DIV(0, 5),
    .sdm        = _SUNXI_CCU_SDM(pll_audio_sdm_table, BIT(31),
                                 0x178, BIT(31)),
    .common     = {
        .reg        = 0x098,
        .features   = CCU_FEATURE_SIGMA_DELTA_MOD,
        .hw.init    = CLK_HW_INIT("pll-audio", "hosc",
                                  &ccu_nm_ops,
                                  CLK_SET_RATE_UNGATE),
    },
};

static CLK_FIXED_FACTOR_HW(pll_audio2x_clk, "pll-audio2x",
                          &pll_audio_clk.common.hw, 2, 1, 0);
static CLK_FIXED_FACTOR_HW(pll_audio1x_clk, "pll-audio1x",
                          &pll_audio_clk.common.hw, 4, 1, 0);

/* DPLL1 Output Configure Register */
static SUNXI_CCU_GATE(ck1_usb_clk, "ck1-usb", "hosc", 0x0a4, BIT(31), 0);

static struct clk_div_table aud_div_table[] =
{
    { .val = 0, .div = 85 },
    { .val = 1, .div = 39 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck1_aud_clk, "ck1-aud", "dpll1",
		0x0a4, 24, 1, aud_div_table, BIT(27), 0);

static struct clk_div_table dev_div_table[] =
{
    { .val = 0, .div = 7 },
    { .val = 1, .div = 6 },
    { .val = 2, .div = 5 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck1_dev_clk, "ck1-dev", "dpll1",
		0x0a4, 20, 2, dev_div_table, BIT(23), 0);

static struct clk_div_table ck1_lspsram_div_table[] =
{
    { .val = 0, .div = 8 },
    { .val = 1, .div = 7 },
    { .val = 2, .div = 6 },
    { .val = 3, .div = 5 },
    { .val = 4, .div = 4 },
    { /* Sentinel */ },
};
static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck1_lspsram_clk, "ck1-lspsram", "dpll1",
		0x0a4, 16, 3, ck1_lspsram_div_table, BIT(19), 0);

static struct clk_div_table hspsram_div_table[] =
{
    { .val = 0, .div = 6 },
    { .val = 1, .div = 5 },
    { .val = 2, .div = 4 },
    { .val = 3, .div = 2 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck1_hspsram_pre_clk, "ck1-hspsram-pre", "dpll1",
		0x0a4, 12, 2, hspsram_div_table, BIT(15), 0);
static CLK_FIXED_FACTOR_FW_NAME(ck1_hspsram_clk, "ck1-hspsram", "ck1-hspsram-pre",
                                 1, 2, CLK_SET_RATE_PARENT);

static struct clk_div_table hifi5_div_table[] =
{
    { .val = 0, .div = 7 },
    { .val = 1, .div = 6 },
    { .val = 2, .div = 5 },
    { .val = 3, .div = 4 },
    { .val = 4, .div = 3 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck1_hifi5_clk, "ck1-hifi5", "dpll1",
		0x0a4, 8, 3, hifi5_div_table, BIT(11), 0);

static struct clk_div_table c906_div_table[] =
{
    { .val = 0, .div = 14 },
    { .val = 1, .div = 8 },
    { .val = 2, .div = 6 },
    { .val = 3, .div = 5 },
    { .val = 4, .div = 4 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck1_c906_pre_clk, "ck1-c906-pre", "dpll1",
		0x0a4, 4, 3, c906_div_table, BIT(7), 0);
static CLK_FIXED_FACTOR_FW_NAME(ck1_c906_clk, "ck1-c906", "ck1-c906-pre",
                                 1, 2, CLK_SET_RATE_PARENT);

static struct clk_div_table m33_div_table[] =
{
    { .val = 0, .div = 8 },
    { .val = 1, .div = 7 },
    { .val = 2, .div = 6 },
    { .val = 3, .div = 5 },
    { .val = 4, .div = 4 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck1_m33_clk, "ck1-m33", "dpll1",
		0x0a4, 0, 3, m33_div_table, BIT(3), 0);

/* DPLL3 Output Configure Register */
static struct clk_div_table ck3_dev_div_table[] =
{
    { .val = 1, .div = 6 },
    { .val = 2, .div = 5 },
    { .val = 3, .div = 4 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck3_dev_clk, "ck3-dev", "dpll3",
		0x0a8, 20, 2, ck3_dev_div_table, BIT(23), 0);

static struct clk_div_table ck3_lspsram_div_table[] =
{
    { .val = 0, .div = 8 },
    { .val = 1, .div = 7 },
    { .val = 2, .div = 6 },
    { .val = 3, .div = 5 },
    { .val = 4, .div = 4 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck3_lspsram_clk, "ck3-lspsram", "dpll3",
		0x0a8, 16, 3, ck3_lspsram_div_table, BIT(19), 0);

static struct clk_div_table ck3_hspsram_div_table[] =
{
    { .val = 0, .div = 6 },
    { .val = 1, .div = 5 },
    { .val = 2, .div = 4 },
    { .val = 3, .div = 2 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck3_hspsram_pre_clk, "ck3-hspsram-pre", "dpll3",
		0x0a8, 12, 2, ck3_hspsram_div_table, BIT(15), 0);
static CLK_FIXED_FACTOR_FW_NAME(ck3_hspsram_clk, "ck3-hspsram", "ck3-hspsram-pre",
                                 1, 2, CLK_SET_RATE_PARENT);

static struct clk_div_table ck3_hifi5_div_table[] =
{
    { .val = 0, .div = 7 },
    { .val = 1, .div = 6 },
    { .val = 2, .div = 5 },
    { .val = 3, .div = 4 },
    { .val = 4, .div = 3 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck3_hifi5_clk, "ck3-hifi5", "dpll3",
		0x0a8, 8, 3, ck3_hifi5_div_table, BIT(11), 0);

static struct clk_div_table ck3_c906_div_table[] =
{
    { .val = 0, .div = 14 },
    { .val = 1, .div = 8 },
    { .val = 2, .div = 6 },
    { .val = 3, .div = 5 },
    { .val = 4, .div = 4 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck3_c906_pre_clk, "ck3-c906-pre", "dpll3",
		0x0a8, 4, 3, ck3_c906_div_table, BIT(7), 0);
static CLK_FIXED_FACTOR_FW_NAME(ck3_c906_clk, "ck3-c906", "ck3-c906-pre",
                                 1, 2, CLK_SET_RATE_PARENT);
static struct clk_div_table ck3_m33_div_table[] =
{
    { .val = 0, .div = 8 },
    { .val = 1, .div = 7 },
    { .val = 2, .div = 6 },
    { .val = 3, .div = 5 },
    { .val = 4, .div = 4 },
    { /* Sentinel */ },
};

static SUNXI_CCU_DIV_TABLE_WITH_GATE(ck3_m33_clk, "ck3-m33", "dpll3",
		0x0a8, 0, 3, ck3_m33_div_table, BIT(3), 0);

/* AUDPLL and DPLL3 LDO Control Register */
static SUNXI_CCU_GATE(ldo_bypass_clk, "ldo-bypass", "hosc", 0x0ac, BIT(1), 0);
static SUNXI_CCU_GATE(ldo2_en_clk, "ldo2-en", "hosc", 0x0ac, BIT(16), 0);
static SUNXI_CCU_GATE(ldo1_en_clk, "ldo1-en", "hosc", 0x0ac, BIT(0), 0);

/* Wlan BT PFTP Contrcl Rigister */
static SUNXI_CCU_GATE(wlan_bt_debug_sel0_clk, "wlan-bt-debug0-sel", "hosc", 0x0c4, BIT(4), 0);
static SUNXI_CCU_GATE(wlan_bt_debug_sel1_clk, "wlan-bt-debug1-sel", "hosc", 0x0c4, BIT(5), 0);
static SUNXI_CCU_GATE(wlan_bt_debug_sel2_clk, "wlan-bt-debug2-sel", "hosc", 0x0c4, BIT(6), 0);
static SUNXI_CCU_GATE(wlan_bt_debug_sel3_clk, "wlan-bt-debug3-sel", "hosc", 0x0c4, BIT(7), 0);
static SUNXI_CCU_GATE(wlan_bt_debug_sel4_clk, "wlan-bt-debug4-sel", "hosc", 0x0c4, BIT(8), 0);
static SUNXI_CCU_GATE(wlan_bt_debug_sel5_clk, "wlan-bt-debug5-sel", "hosc", 0x0c4, BIT(9), 0);
static SUNXI_CCU_GATE(wlan_bt_debug_sel6_clk, "wlan-bt-debug6-sel", "hosc", 0x0c4, BIT(10), 0);
static SUNXI_CCU_GATE(wlan_bt_debug_sel7_clk, "wlan-bt-debug7-sel", "hosc", 0x0c4, BIT(11), 0);
static const char *const wlan_bt_sel_parents[] = { "rfip0-dpll", "rfip1-dpll" };
static SUNXI_CCU_MUX(wlan_sel_clk, "wlan_sel", wlan_bt_sel_parents, 0x0c4, 2, 1, 0);
static SUNXI_CCU_MUX(bt_sel_clk, "bt_sel", wlan_bt_sel_parents, 0x0c4, 1, 1, 0);
static SUNXI_CCU_GATE(pfip2_gate_clk, "pfip2-gate", "hosc", 0x0c4, BIT(0), 0);

/* Module Clock Enable Register */
static SUNXI_CCU_GATE(ble_32m_clk, "ble-32m", "hosc", 0x0cc, BIT(17), 0);
static SUNXI_CCU_GATE(ble_48m_clk, "ble-48m", "hosc", 0x0cc, BIT(16), 0);
static SUNXI_CCU_GATE(mad_ahb_gate_clk, "mad_ahb_gate", "sys", 0x0cc, BIT(15), 0);
static SUNXI_CCU_GATE(gpio_gate_clk, "gpio_gate", "hosc", 0x0cc, BIT(11), 0);
static SUNXI_CCU_GATE(bus_codec_dac_clk, "bus_codec_dac", "hosc", 0x0cc, BIT(10), 0);
static SUNXI_CCU_GATE(rccal_clk, "rccal", "hosc", 0x0cc, BIT(8), 0);
static SUNXI_CCU_GATE(bus_codec_adc_clk, "bus_codec_adc", "hosc", 0x0cc, BIT(5), 0);
static SUNXI_CCU_GATE(mad_apb_gate_clk, "mad_apb_gate", "apb", 0x0cc, BIT(4), 0);
static SUNXI_CCU_GATE(dmic_bus_clk, "dmic-bus", "hosc", 0x0cc, BIT(3), 0);
static SUNXI_CCU_GATE(gpadc_clk, "gpadc", "hosc", 0x0cc, BIT(2), 0);
static SUNXI_CCU_GATE(lpuart1_wkup_clk, "lpuart1-wkup", "hosc", 0x0cc, BIT(1), 0);
static SUNXI_CCU_GATE(lpuart0_wkup_clk, "lpuart0-wkup", "hosc", 0x0cc, BIT(0), 0);

/* LPUART0 Control Register */
static const char *const lpuart0_parents[] = { "fix-losc", "hosc" };
static SUNXI_CCU_MUX_WITH_GATE(lpuart0_clk, "lpuart0", lpuart0_parents, 0x0d0, 0, 1, BIT(31), 0);

/* LPUART1 Control Register */
static const char *const lpuart1_parents[] = { "fix-losc", "hosc" };
static SUNXI_CCU_MUX_WITH_GATE(lpuart1_clk, "lpuart1", lpuart1_parents, 0x0d4, 0, 1, BIT(31), 0);

/* GPADC Clock Control Register */
static const char *const gpadc_parents[] = { "hosc", "fix-losc" };
static SUNXI_CCU_MP_WITH_MUX_GATE(gpadc_ctrl_clk, "gpadc_ctrl", gpadc_parents, 0x0d8,
                                  0, 4,     /* M */
                                  16, 2,     /* N */
                                  24, 2,    /* mux */
                                  BIT(31),  /* gate */
                                  0);

/* Audio Clock Control Register */
static const char *const spdif_tx_parents[] = { "audpll-hosc-sel", "ck1-aud-div" };
static SUNXI_CCU_MUX_WITH_GATE(spdif_tx_clk, "spdif-tx", spdif_tx_parents, 0x0dc, 21, 1, BIT(27), 0);
static const char *const i2s_parents[] = { "audpll-hosc-sel", "ck1-aud-div" };
static SUNXI_CCU_MUX_WITH_GATE(i2s_clk, "i2s", i2s_parents, 0x0dc, 20, 1, BIT(26), 0);
static const char *const codec_dac_parents[] = { "audpll-hosc-sel", "ck1-aud-div" };
static SUNXI_CCU_MUX_WITH_GATE(codec_dac_clk, "codec-dac", codec_dac_parents, 0x0dc, 19, 1, BIT(25), 0);

static const char *const codec_adc_sel1_parents[] = { "codec-adc-div", "aud-rco-div" };
static SUNXI_CCU_MUX(codec_adc_sel1_clk, "codec-adc-sel1", codec_adc_sel1_parents, 0x0dc, 18, 1, 0);
static SUNXI_CCU_GATE(codec_adc_gate_clk, "codec-adc-gate", "hosc", 0x0dc, BIT(24), 0);

static SUNXI_CCU_GATE(dmic_gate_clk, "dmic-gate", "hosc", 0x0dc, BIT(28), 0);

static const char *const audpll_parents[] = { "pll-audio1x", "pll-audio2x", "hosc", "daudio-mclk-in" };
static SUNXI_CCU_MUX(audpll_hosc_sel_clk, "audpll-hosc-sel", audpll_parents, 0x0dc, 15, 2, 0);

static const char *const codec_adc_div_parents[] = { "audpll-hosc-sel", "ck1-aud-div" };
static SUNXI_CCU_M_WITH_MUX(codec_adc_div_clk, "codec-adc-div",
                           codec_adc_div_parents, 0x0dc, 8, 4,
                          17, 1, 0);

static SUNXI_CCU_M(ck1_aud_div_clk, "ck1-aud-div",
                           "ck1-aud", 0x0dc, 4, 4, 0);

static SUNXI_CCU_M(aud_rco_div_clk, "aud-rco-div",
                           "rc-hf", 0x0dc, 0, 3, 0);

/* System Clock Control Register */
static const char *const hspsram_clk_parents[] = { "ck1-hspsram", "ck3-hspsram" };
static SUNXI_CCU_MUX(ckpll_hspsram_sel_clk, "ckpll-hspsram-sel", hspsram_clk_parents, 0x0e0, 21, 1, 0);

static const char *const lspsram_clk_parents[] = { "ck1-lspsram", "ck3-lspsram" };
static SUNXI_CCU_MUX(ckpll_lspsram_sel_clk, "ckpll-lspsram-sel", lspsram_clk_parents, 0x0e0, 20, 1, 0);

static const char *const ck_m33_parents[] = { "ck1-m33", "ck3-m33" };
static SUNXI_CCU_MUX(ck_m33_clk, "ck-m33", ck_m33_parents, 0x0e0, 19, 1, 0);

static const char *const ck_hifi5_parents[] = { "ck1-hifi5", "ck3-hifi5" };
static SUNXI_CCU_MUX(ckpll_hifi5_sel_clk, "ckpll-hifi5-sel", ck_hifi5_parents, 0x0e0, 18, 1, 0);

static const char *const ck_c906_parents[] = { "ck1-c906", "ck3-c906" };
static SUNXI_CCU_MUX(ckpll_c906_sel_clk, "ckpll-c906-sel", ck_c906_parents, 0x0e0, 17, 1, 0);

static const char *const ck_dev_parents[] = { "ck1-dev", "ck3-dev" };
static SUNXI_CCU_MUX(ck_dev_clk, "ck-dev", ck_dev_parents, 0x0e0, 16, 1, 0);

static SUNXI_CCU_M(sys_clk, "sys", "ck-m33", 0x0e0, 8, 4, 0);

static const char *const ar200a_parents[] = { "hosc", "fix-losc", "sys", "aud-rco-div" };
static SUNXI_CCU_MUX(ar200a_f_clk, "ar200a-f", ar200a_parents, 0x0e0, 12, 2, 0);


static struct clk_div_table apb_div_table[] =
{
    { .val = 0, .div = 1 },
    { .val = 1, .div = 2 },
    { .val = 2, .div = 4 },
    { .val = 3, .div = 8 },
    { /* Sentinel */ },
};
static SUNXI_CCU_DIV_TABLE(hfclk_div_clk, "hfclk-div", "hosc", 0x0e0,
			   4, 2, apb_div_table, 0);

static SUNXI_CCU_DIV_TABLE(lfclk_div_clk, "lfclk-div", "fix-losc", 0x0e0,
			   4, 2, apb_div_table, 0);

static SUNXI_CCU_DIV_TABLE(ahb_div_clk, "ahb-div", "ar200a-f", 0x0e0,
			   4, 2, apb_div_table, 0);

static const char *apb_parents[] = { "hfclk-div", "lfclk-div",
				     "ahb-div", "aud-rco-div" };
static SUNXI_CCU_MUX(apb_clk, "apb", apb_parents,
		     0x0e0,
		     6, 2, /* mux */
		     CLK_IS_CRITICAL);

static SUNXI_CCU_M(device_clk, "device-clk",
                           "ck-dev", 0x0e0, 0, 4,
			   CLK_IS_CRITICAL);

/* MAD_lpsd_clk control register */
/* there is no actual mux,just for use this macro */
static const char *const mad_lpsd_clk_parents[] = { "codec-adc-sel1" };
static SUNXI_CCU_MP_WITH_MUX_GATE(mad_lpsd_clk, "mad-lpsd", mad_lpsd_clk_parents, 0x0e4,
                                  0, 4,     /* M */
                                  16, 2,     /* N */
                                  24, 2,    /* mux */
                                  BIT(31),  /* gate */
                                  0);

/* SPDIF_RX_clock control register */
static const char *const spdif_rx_parents[] = { "ck1-m33", "ckpll-hifi5-sel", "ckpll-c906-sel", "ck3-m33" };
static SUNXI_CCU_MP_WITH_MUX_GATE(spdif_rx_clk, "spdif-rx", spdif_rx_parents, 0x0e8,
                                  0, 4,     /* M */
                                  16, 2,     /* N */
                                  24, 2,    /* mux */
                                  BIT(31),  /* gate */
                                  0);

/* I2S_ASRC_clk control register */
static const char *const i2s_asrc_parents[] = { "ck-m33", "ckpll-hifi5-sel", "ckpll-c906-sel" ,"ck3-hifi5" };
static SUNXI_CCU_MP_WITH_MUX_GATE(i2s_asrc_clk, "i2s-asrc", i2s_asrc_parents, 0x0ec,
                                  0, 4,     /* M */
                                  16, 2,     /* N */
                                  24, 2,    /* mux */
                                  BIT(31),  /* gate */
                                  0);

static struct ccu_common *sun20iw2_ccu_on_clks[] =
{
    &hosc_clk.common,
    &hosc_detect_clk.common,
    &dpll1_clk.common,
    &dpll2_clk.common,
    &dpll3_clk.common,
    &pll_audio_clk.common,
    &ck1_usb_clk.common,
    &ck1_aud_clk.common,
    &ck1_dev_clk.common,
    &ck1_lspsram_clk.common,
    &ck1_hspsram_pre_clk.common,
    &ck1_hifi5_clk.common,
    &ck1_c906_pre_clk.common,
    &ck1_m33_clk.common,
    &ck3_dev_clk.common,
    &ck3_lspsram_clk.common,
    &ck3_hspsram_pre_clk.common,
    &ck3_hifi5_clk.common,
    &ck3_c906_pre_clk.common,
    &ck3_m33_clk.common,
    &ldo_bypass_clk.common,
    &ldo2_en_clk.common,
    &ldo1_en_clk.common,
    &wlan_bt_debug_sel0_clk.common,
    &wlan_bt_debug_sel1_clk.common,
    &wlan_bt_debug_sel2_clk.common,
    &wlan_bt_debug_sel3_clk.common,
    &wlan_bt_debug_sel4_clk.common,
    &wlan_bt_debug_sel5_clk.common,
    &wlan_bt_debug_sel6_clk.common,
    &wlan_bt_debug_sel7_clk.common,
    &wlan_sel_clk.common,
    &bt_sel_clk.common,
    &pfip2_gate_clk.common,
    &ble_32m_clk.common,
    &ble_48m_clk.common,
    &mad_ahb_gate_clk.common,
    &gpio_gate_clk.common,
    &bus_codec_dac_clk.common,
    &rccal_clk.common,
    &bus_codec_adc_clk.common,
    &mad_apb_gate_clk.common,
    &dmic_bus_clk.common,
    &gpadc_clk.common,
    &lpuart1_wkup_clk.common,
    &lpuart0_wkup_clk.common,
    &lpuart0_clk.common,
    &lpuart1_clk.common,
    &gpadc_ctrl_clk.common,
    &dmic_gate_clk.common,
    &spdif_tx_clk.common,
    &i2s_clk.common,
    &codec_dac_clk.common,
    &codec_adc_sel1_clk.common,
    &codec_adc_gate_clk.common,
    &audpll_hosc_sel_clk.common,
    &codec_adc_div_clk.common,
    &ck1_aud_div_clk.common,
    &aud_rco_div_clk.common,
    &ckpll_hspsram_sel_clk.common,
    &ckpll_lspsram_sel_clk.common,
    &ck_m33_clk.common,
    &ckpll_hifi5_sel_clk.common,
    &ckpll_c906_sel_clk.common,
    &ck_dev_clk.common,
    &sys_clk.common,
    &ar200a_f_clk.common,
    &hfclk_div_clk.common,
    &lfclk_div_clk.common,
    &ahb_div_clk.common,
    &apb_clk.common,
    &device_clk.common,
    &mad_lpsd_clk.common,
    &spdif_rx_clk.common,
    &i2s_asrc_clk.common,
};
/* ccu_def_start */
static struct clk_hw_onecell_data sun20iw2_ccu_on_hw_clks =
{
    .hws    = {
	[CLK_HOSC]          = &hosc_clk.common.hw,
	[CLK_HOSC_DETECT]	= &hosc_detect_clk.common.hw,
	[CLK_DPLL1]         = &dpll1_clk.common.hw,
	[CLK_DPLL2]         = &dpll2_clk.common.hw,
	[CLK_DPLL3]         = &dpll3_clk.common.hw,
	[RFIP0_DPLL]         = &rfip0_dpll.hw,
	[RFIP1_DPLL]         = &rfip1_dpll.hw,
	[CLK_PLL_AUDIO]     = &pll_audio_clk.common.hw,
	[CLK_PLL_AUDIO1X]   = &pll_audio1x_clk.hw,
	[CLK_PLL_AUDIO2X]   = &pll_audio2x_clk.hw,
	[CLK_CK1_USB]	    = &ck1_usb_clk.common.hw,
	[CLK_CK1_AUD]	    = &ck1_aud_clk.common.hw,
	[CLK_CK1_DEV]       = &ck1_dev_clk.common.hw,
	[CLK_CK1_LSPSRAM]   = &ck1_lspsram_clk.common.hw,
	[CLK_CK1_HSPSRAM_PRE]   = &ck1_hspsram_pre_clk.common.hw,
	[CLK_CK1_HSPSRAM]   = &ck1_hspsram_clk.hw,
	[CLK_CK1_HIFI5]     = &ck1_hifi5_clk.common.hw,
	[CLK_CK1_C906_PRE]		= &ck1_c906_pre_clk.common.hw,
	[CLK_CK1_C906]		= &ck1_c906_clk.hw,
	[CLK_CK1_M33]		= &ck1_m33_clk.common.hw,
	[CLK_CK3_DEV]		= &ck3_dev_clk.common.hw,
	[CLK_CK3_LSPSRAM]	= &ck3_lspsram_clk.common.hw,
	[CLK_CK3_HSPSRAM_PRE]	= &ck3_hspsram_pre_clk.common.hw,
	[CLK_CK3_HSPSRAM]	= &ck3_hspsram_clk.hw,
	[CLK_CK3_HIFI5]		= &ck3_hifi5_clk.common.hw,
	[CLK_CK3_C906_PRE]		= &ck3_c906_pre_clk.common.hw,
	[CLK_CK3_C906]		= &ck3_c906_clk.hw,
	[CLK_CK3_M33]		= &ck3_m33_clk.common.hw,
	[CLK_LDO_BYPASS]      = &ldo_bypass_clk.common.hw,
	[CLK_LDO2_EN]          = &ldo2_en_clk.common.hw,
	[CLK_LDO1_EN]          = &ldo1_en_clk.common.hw,
	[CLK_WLAN_BT_DEBUG_SEL0]	= &wlan_bt_debug_sel0_clk.common.hw,
	[CLK_WLAN_BT_DEBUG_SEL1]	= &wlan_bt_debug_sel1_clk.common.hw,
	[CLK_WLAN_BT_DEBUG_SEL2]	= &wlan_bt_debug_sel2_clk.common.hw,
	[CLK_WLAN_BT_DEBUG_SEL3]	= &wlan_bt_debug_sel3_clk.common.hw,
	[CLK_WLAN_BT_DEBUG_SEL4]	= &wlan_bt_debug_sel4_clk.common.hw,
	[CLK_WLAN_BT_DEBUG_SEL5]	= &wlan_bt_debug_sel5_clk.common.hw,
	[CLK_WLAN_BT_DEBUG_SEL6]	= &wlan_bt_debug_sel6_clk.common.hw,
	[CLK_WLAN_BT_DEBUG_SEL7]	= &wlan_bt_debug_sel7_clk.common.hw,
	[CLK_WLAN_SEL]		= &wlan_sel_clk.common.hw,
	[CLK_BT_SEL]		= &bt_sel_clk.common.hw,
	[CLK_PFIP2_GATE]	= &pfip2_gate_clk.common.hw,
	[CLK_BLE_32M]         = &ble_32m_clk.common.hw,
	[CLK_BLE_48M]         = &ble_48m_clk.common.hw,
	[CLK_MAD_AHB_GATE]    = &mad_ahb_gate_clk.common.hw,
	[CLK_GPIO_GATE]       = &gpio_gate_clk.common.hw,
	[CLK_BUS_CODEC_DAC]   = &bus_codec_dac_clk.common.hw,
	[CLK_RCCAL]           = &rccal_clk.common.hw,
	[CLK_BUS_CODEC_ADC]   = &bus_codec_adc_clk.common.hw,
	[CLK_MAD_APB_GATE]    = &mad_apb_gate_clk.common.hw,
	[CLK_BUS_DMIC]       = &dmic_bus_clk.common.hw,
	[CLK_GPADC]           = &gpadc_clk.common.hw,
	[CLK_LPUART1_WKUP]    = &lpuart1_wkup_clk.common.hw,
	[CLK_LPUART0_WKUP]    = &lpuart0_wkup_clk.common.hw,
	[CLK_LPUART0]	      = &lpuart0_clk.common.hw,
	[CLK_LPUART1]         = &lpuart1_clk.common.hw,
	[CLK_GPADC_CTRL]      = &gpadc_ctrl_clk.common.hw,
	[CLK_DMIC_GATE]            = &dmic_gate_clk.common.hw,
	[CLK_SPDIF_TX]        = &spdif_tx_clk.common.hw,
	[CLK_I2S]             = &i2s_clk.common.hw,
	[CLK_CODEC_DAC]       = &codec_dac_clk.common.hw,
	[CLK_CODEC_ADC_SEL1]       = &codec_adc_sel1_clk.common.hw,
	[CLK_CODEC_ADC_GATE]       = &codec_adc_gate_clk.common.hw,
	[CLK_AUDPLL_HOSC_SEL] = &audpll_hosc_sel_clk.common.hw,
	[CLK_CODEC_ADC_DIV]   = &codec_adc_div_clk.common.hw,
	[CLK_CK1_AUD_DIV]	= &ck1_aud_div_clk.common.hw,
	[CLK_AUD_RCO_DIV]     = &aud_rco_div_clk.common.hw,
	[CLK_CKPLL_HSPSRAM_SEL] = &ckpll_hspsram_sel_clk.common.hw,
	[CLK_CKPLL_LSPSRAM_SEL] = &ckpll_lspsram_sel_clk.common.hw,
	[CLK_CK_M33]		= &ck_m33_clk.common.hw,
	[CLK_CKPLL_HIFI5_SEL]	= &ckpll_hifi5_sel_clk.common.hw,
	[CLK_CKPLL_C906_SEL]    = &ckpll_c906_sel_clk.common.hw,
	[CLK_CK_DEV]		= &ck_dev_clk.common.hw,
	[CLK_SYS]		= &sys_clk.common.hw,
	[CLK_AR200A_F]		= &ar200a_f_clk.common.hw,
	[CLK_HFCLK_DIV]		= &hfclk_div_clk.common.hw,
	[CLK_LFCLK_DIV]		= &lfclk_div_clk.common.hw,
	[CLK_AHB_DIV]		= &ahb_div_clk.common.hw,
	[CLK_APB]		= &apb_clk.common.hw,
	[CLK_DEVICE]		= &device_clk.common.hw,
	[CLK_MAD_LPSD]        = &mad_lpsd_clk.common.hw,
	[CLK_SPDIF_RX]	      = &spdif_rx_clk.common.hw,
	[CLK_I2S_ASRC]        = &i2s_asrc_clk.common.hw,
    },
    .num = CLK_AON_NUMBER,
};
/* ccu_def_end */
static const u32 pll_regs[] =
{
    SUN20IW2_PLL_AUDIO_REG,
};

static const struct sunxi_ccu_desc sun20iw2_ccu_on_desc =
{
    .ccu_clks   = sun20iw2_ccu_on_clks,
    .num_ccu_clks   = ARRAY_SIZE(sun20iw2_ccu_on_clks),


    .hw_clks    = &sun20iw2_ccu_on_hw_clks,
    .clk_type   = HAL_SUNXI_AON_CCU,

    .resets     = sun20iw2_ccu_on_resets,
    .reset_type = HAL_SUNXI_AON_RESET,
    .num_resets = ARRAY_SIZE(sun20iw2_ccu_on_resets),
};

#define HOSC_CLOCK_24M      (24U * 1000U * 1000U)
#define HOSC_CLOCK_26M      (26U * 1000U * 1000U)
#define HOSC_CLOCK_32M      (32U * 1000U * 1000U)
#define HOSC_CLOCK_40M      (40U * 1000U * 1000U)
#define HOSC_CLOCK_24_576M      (24576U * 1000U)

uint32_t HAL_GetHFClock(void)
{
	static const uint32_t PRCM_HOSCClock[] = {
			HOSC_CLOCK_26M,
			HOSC_CLOCK_40M,
			HOSC_CLOCK_24M,
			HOSC_CLOCK_32M,
			HOSC_CLOCK_24_576M};
	uint32_t val = 0;

	val = *(volatile uint32_t *)(SUNXI20_CCU_AON_BASE + 0x84);

	return PRCM_HOSCClock[val];
}

int sunxi_ccu_aon_init(void)
{
    unsigned long reg;
    int ret;
    u32 val;
    int i;

    /* HOSC Type Register, set it to 40M */
    u32 hosc_reg;
    hosc_reg = (u32)readl(0x4004c484);
    hosc_reg = 0x1;
    writel(hosc_reg, 0x4004c484);

    for (i = 0; i < ARRAY_SIZE(pll_regs); i++)
    {
        val = readl(pll_regs[i]);
        val |= BIT(29);
        writel(val, pll_regs[i]);
    }

    reg = (unsigned long)SUNXI20_CCU_AON_BASE;
    ret = ccu_common_init(reg, &sun20iw2_ccu_on_desc);
    return ret;
}
