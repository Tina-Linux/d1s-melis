/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY��S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS��SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY��S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __DBI_SUN20IW2_H__
#define __DBI_SUN20IW2_H__

#include "../platform_dbi.h"
#include "../common_dbi.h"

#define SUNXI_DBI0_PBASE         (0x40009000ul) /* 4K */
#define SUNXI_DBI1_PBASE         (0x4000F000ul) /* 4K */
#define SUNXI_IRQ_DBI0                 31

#ifdef CONFIG_ARCH_RISCV_C906
#define SUNXI_IRQ_DBI1                 62  // rv
#else
#define SUNXI_IRQ_DBI1                 46  // m33
#endif

#define DBI_MAX_NUM 2
#define DBI0_PARAMS \
{.reg_base = SUNXI_DBI0_PBASE, .irq_num = SUNXI_IRQ_DBI0, .gpio_num = 6, \
	.gpio_clk = GPIOC(2), .gpio_mosi = GPIOC(4), .gpio_miso = GPIOC(5), \
	.gpio_cs0 = GPIOC(3), .gpio_wp = GPIOC(6), .gpio_hold = GPIOC(7), \
	.mux = 2, .driv_level = GPIO_DRIVING_LEVEL2}
#define DBI1_PARAMS \
{.reg_base = SUNXI_DBI1_PBASE, .irq_num = SUNXI_IRQ_DBI1, .gpio_num = 6, \
	.gpio_clk = GPIOA(13), .gpio_mosi = GPIOA(18), .gpio_miso = GPIOA(21), \
	.gpio_cs0 = GPIOA(12), .gpio_wp = GPIOA(20), .gpio_hold = GPIOA(19), \
	.mux = 6, .driv_level = GPIO_DRIVING_LEVEL0}

#define CLK_SPI2 0//RESEVER
#define CLK_BUS_SPI2 0//RESEVER
#define RST_BUS_SPI2 0//RESEVER
#define SUNXI_CLK_SPI(x)	CLK_SPI##x
#define SUNXI_CLK_RST_SPI(x)	RST_SPI##x
#define SUNXI_CLK_BUS_SPI(x)	CLK_BUS_SPI##x
#define SUNXI_CLK_PLL_SPI CLK_DEVICE
#define SUNXI_CLK_TYPE_PCLK_SPI      HAL_SUNXI_AON_CCU

/* regs structs */
union dbi_reservd_reg_t {
	u32 dwval;
	struct {
		u32 res0;
	} bits;
};

union spi_gcr_reg_t {
	u32 dwval;
	struct {
		u32 spi_en:1;
		u32 mode:1;  // 1: master, 0: slave
		u32 mode_select:1;
		u32 dbi_mode_sel:1;
		u32 dbi_en:1;
		u32 res0:2;
		u32 tp_en:1;  // transmit pause enable
		u32 res1:23;
		u32 srst:1;  // soft reset
	} bits;
};

union spi_trans_ctrl {
	u32 dwval;
	struct {
		u32 cpha:1;
		u32 cpol:1;
		u32 spol:1;
		u32 ssctl:1;
		u32 ss_sel:2;
		u32 ss_owner:1;
		u32 ss_level:1;
		u32 dhb:1;
		u32 ddb:1;
		u32 rpsm:1;
		u32 sdc:1;
		u32 fbs:1;
		u32 sdm:1;
		u32 sddm:1;
		u32 sdc1:1;
		u32 res0:15;
		u32 xch:1;
	} bits;
};

union dbi_mbc_reg_t {
	u32 dwval;
	struct {
		u32 mbc:24;
		u32 res:8;
	} bits;
};

/* master transmit counter */
union dbi_mtc_reg_t {
	u32 dwval;
	struct {
		u32 mwtc:24;
		u32 res:8;
	} bits;
};

/* master burst control counter */
union dbi_bcc_reg_t {
	u32 dwval;
	struct {
		u32 stc:24;
		u32 dbc:4;
		u32 drm:1;
		u32 quad_en:1;
		u32 res0:2;
	} bits;
};

union dbi_ctrl_reg0_t {
	u32 dwval;
	struct {
		u32 video_src_type:1;
		u32 element_a_position:1;
		u32 rgb_bit_order:1;
		u32 dummy_cycle_value:1;
		u32 rgb_src_fmt:4;
		u32 dbi_interface:3;
		u32 res0:1;
		u32 output_format:3;
		u32 transmit_mode:1;
		u32 output_rgb_seq:3;
		u32 output_data_seq:1;
		u32 write_cmd_dummy_cycle:11;
		u32 cmd_type:1;
	} bits;
};

union dbi_ctrl_reg_t {
	u32 dwval;
	struct {
		u32 read_data_num:8;
		u32 read_cmd_cycle:8;
		u32 res0:4;
		u32 read_msb_first:1;
		u32 data_src_sel:1;
		u32 dcx_data:1;
		u32 clk_output_inv:1;
		u32 output_clk_mode:1;
		u32 rx_clk_inv:1;
		u32 rgb666_fmt:2;
		u32 res1:1;
		u32 mode_sel:2;
		u32 soft_tri:1;
	} bits;
};

union dbi_int_reg_t {
	u32 dwval;
	struct {
		u32 line_done_int_en:1;
		u32 frame_done_int_en:1;
		u32 te_int_en:1;
		u32 rd_done_int_en:1;
		u32 timer_int_en:1;
		u32 dbi_fifo_full_int_en:1;
		u32 dbi_fifo_empty_int_en:1;
		u32 res0:1;
		u32 line_done_int:1;
		u32 frame_done_int:1;
		u32 te_int:1;
		u32 rd_done_int:1;
		u32 timer_int:1;
		u32 dbi_fifo_full_int:1;
		u32 dbi_fifo_empty_int:1;
		u32 res1:17;
	} bits;
};

struct sunxi_dbi_dev {
	union dbi_reservd_reg_t dbi_reg_000;
	union spi_gcr_reg_t gc;
	union spi_trans_ctrl trans_ctrl;
	union dbi_reservd_reg_t dbi_reg_00c;

	union dbi_reservd_reg_t dbi_reg_010;
	union dbi_reservd_reg_t dbi_reg_014;
	union dbi_reservd_reg_t dbi_reg_018;
	union dbi_reservd_reg_t dbi_reg_01c;

	union dbi_reservd_reg_t dbi_reg_020;
	union dbi_reservd_reg_t dbi_reg_024;
	union dbi_reservd_reg_t dbi_reg_028;
	union dbi_reservd_reg_t dbi_reg_02c;

	union dbi_mbc_reg_t mbc;
	union dbi_mtc_reg_t mwtc;
	union dbi_bcc_reg_t bcc;
	union dbi_reservd_reg_t dbi_reg_03c;

	union dbi_reservd_reg_t dbi_reg_040[48]; /* 0x40 ~ 0xfc */

	// 0x100
	union dbi_ctrl_reg0_t dbi_ctrl0;
	union dbi_ctrl_reg_t dbi_ctrl;
	union dbi_reservd_reg_t dbi_reg_108;
	union dbi_reservd_reg_t dbi_reg_10c;
	union dbi_reservd_reg_t dbi_reg_110;
	union dbi_reservd_reg_t dbi_reg_114;
	union dbi_reservd_reg_t dbi_reg_118;
	union dbi_reservd_reg_t dbi_reg_11c;

	union dbi_int_reg_t dbi_int;
	union dbi_reservd_reg_t dbi_reg_124;
	union dbi_reservd_reg_t dbi_reg_128;
	union dbi_reservd_reg_t dbi_reg_12c;
};

enum dbi_mode_type;

unsigned int dbi_dev_get_base(u32 sel);
bool dbi_dev_set_base(u32 sel, uintptr_t base);
bool set_chip_select_control(u32 sel);
bool is_master_mode(u32 sel);
bool set_master_counter(u32 sel, u32 tx_len);
bool config_dbi_enable_mode(u32 sel, u32 mode);

bool set_dcx(u32 sel, u32 dcx);
bool config_dbi_output_clk_mode(u32 sel, u32 mode);
bool config_dbi_data_src_sel(u32 sel, u32 arg);
void default_config(u32 sel);
bool config_dbi_output_format(u32 sel, u32 format);
bool set_dbi_en_mode(u32 sel, u32 mode);
bool dbi_irq_enable(u32 sel, enum dbi_mode_type mode_type);
bool dbi_irq_disable(u32 sel, enum dbi_mode_type mode_type);

#endif /*__SPI_SUN8IW20_H__  */

