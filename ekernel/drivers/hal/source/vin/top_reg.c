/*
 * top_reg.c for all v4l2 subdev manage
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <sunxi_hal_common.h>
#include "top_reg_i.h"
#include "top_reg.h"

#include "utility/vin_io.h"

/*isp_id isp_input pasrer_id parser_ch*/
static int isp_input[4][4][4][4] = {
#if defined(CONFIG_ARCH_SUN50IW3P1) || defined(CONFIG_ARCH_SUN50IW6P1)

	/*isp0 input0~3*/
		/*parser0*/     /*parse1*/    /*parser2*/    /*parser3*/
	{
		{{0, 0, 0, 0}, {1, 2, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },
		{{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },
		{{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },
		{{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0} }
	},

	/*isp1 input0~3*/
	{
		{{1, 2, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },
		{{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },
		{{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },
		{{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0} }
	},
#elif defined (CONFIG_ARCH_SUN8IW15P1) || defined (CONFIG_ARCH_SUN8IW17P1) || defined (CONFIG_ARCH_SUN8IW16P1)
	/*isp0 input0~3*/
	{
		{{0, 4, 0, 0}, {1, 5, 0, 0}, {2, 6, 0, 0}, {3, 7, 0, 0} },
		{{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 2, 0, 0}, {0, 3, 0, 0} },
		{{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 2, 0}, {0, 0, 3, 0} },
		{{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 0, 2}, {0, 0, 0, 3} }
	},

	/*isp1 input0~3*/
	{
		{{0, 4, 0, 0}, {1, 5, 0, 0}, {2, 6, 0, 0}, {3, 7, 0, 0} },
		{{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 2, 0, 0}, {0, 3, 0, 0} },
		{{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 2, 0}, {0, 0, 3, 0} },
		{{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 0, 2}, {0, 0, 0, 3} }
	},

	/*isp2 input0~3*/
	{
		{{0, 4, 0, 0}, {1, 5, 0, 0}, {2, 6, 0, 0}, {3, 7, 0, 0} },
		{{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 2, 0, 0}, {0, 3, 0, 0} },
		{{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 2, 0}, {0, 0, 3, 0} },
		{{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 0, 2}, {0, 0, 0, 3} }
	},

	/*isp3 input0~3*/
	{
		{{0, 4, 0, 0}, {1, 5, 0, 0}, {2, 6, 0, 0}, {3, 7, 0, 0} },
		{{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 2, 0, 0}, {0, 3, 0, 0} },
		{{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 2, 0}, {0, 0, 3, 0} },
		{{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 0, 0, 2}, {0, 0, 0, 3} }
	}
#else
	/*isp0 input0~3*/
	{
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} }
	},

	/*isp1 input0~3*/
	{
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} }
	},

	/*isp2 input0~3*/
	{
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} }
	},

	/*isp3 input0~3*/
	{
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
		{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} }
	},
#endif
};

/*vipp_id isp_id isp_ch*/
static int vipp_input[8][4][4] = {
#if defined(CONFIG_ARCH_SUN50IW3P1) || defined(CONFIG_ARCH_SUN50IW6P1)
	/*vipp0*/
	/*isp0*/        /*isp1*/      /*isp2*/       /*isp3*/
	{{0, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },

	{{0, 2, 0, 0}, {1, 3, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },

	{{0, 0, 2, 0}, {1, 0, 3, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },

	{{0, 0, 0, 2}, {1, 4, 0, 3}, {0, 0, 0, 0}, {0, 0, 0, 0} },
#elif defined (CONFIG_ARCH_SUN8IW15P1) || defined (CONFIG_ARCH_SUN8IW17P1) || defined (CONFIG_ARCH_SUN8IW16P1)
	{{0, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },

	{{0, 2, 0, 0}, {1, 3, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },

	{{0, 0, 2, 0}, {1, 0, 3, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },

	{{0, 0, 0, 2}, {1, 4, 0, 3}, {0, 0, 0, 0}, {0, 0, 0, 0} },

	{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 0, 0, 0} },

	{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 2, 0, 0}, {1, 3, 0, 0} },

	{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 2, 0}, {1, 0, 3, 0} },

	{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 2}, {1, 4, 0, 3} }
#else
	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },

	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },

	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },

	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },

	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },

	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },

	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },

	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} },
#endif
};

unsigned long csic_top_base;
unsigned long csic_ccu_base;

int csic_top_set_base_addr(unsigned long addr)
{
	csic_top_base = addr;

	return 0;
}

void csic_top_enable(void)
{
	vin_reg_clr_set(csic_top_base + CSIC_TOP_EN_REG_OFF,
			CSIC_TOP_EN_MASK, 1 << CSIC_TOP_EN);
}

void csic_top_disable(void)
{
	vin_reg_clr_set(csic_top_base + CSIC_TOP_EN_REG_OFF,
			CSIC_TOP_EN_MASK, 0 << CSIC_TOP_EN);
}

void csic_isp_bridge_enable(void)
{
	vin_reg_clr_set(csic_top_base + CSIC_TOP_EN_REG_OFF,
			CSIC_ISP_BRIDGE_EN_MASK, 1 << CSIC_ISP_BRIDGE_EN);
}

void csic_isp_bridge_disable(void)
{
	vin_reg_clr_set(csic_top_base + CSIC_TOP_EN_REG_OFF,
			CSIC_ISP_BRIDGE_EN_MASK, 0 << CSIC_ISP_BRIDGE_EN);
}

void csic_top_sram_pwdn(unsigned int en)
{
	vin_reg_clr_set(csic_top_base + CSIC_TOP_EN_REG_OFF,
			CSIC_SRAM_PWDN_MASK, en << CSIC_SRAM_PWDN);
}

void csic_top_version_read_en(unsigned int en)
{
	vin_reg_clr_set(csic_top_base + CSIC_TOP_EN_REG_OFF,
			CSIC_VER_EN_MASK, en << CSIC_VER_EN);
}

void csic_isp_input_select(unsigned int isp, unsigned int in,
				unsigned int psr, unsigned int ch)
{
	vin_reg_writel(csic_top_base + CSIC_ISP0_IN0_REG_OFF + isp * 16 + in * 4,
			isp_input[isp][in][psr][ch]);
}

void csic_vipp_input_select(unsigned int vipp,
				unsigned int isp, unsigned int ch)
{
	vin_reg_writel(csic_top_base + CSIC_VIPP0_IN_REG_OFF + vipp * 4,
			vipp_input[vipp][isp][ch]);
}

void csic_feature_list_get(struct csic_feature_list *fl)
{
	unsigned int reg_val = 0;
	reg_val = vin_reg_readl(csic_top_base + CSIC_FEATURE_REG_OFF);
	fl->dma_num = (reg_val & CSIC_DMA_NUM_MASK) >> CSIC_DMA_NUM;
	fl->vipp_num = (reg_val & CSIC_VIPP_NUM_MASK) >> CSIC_VIPP_NUM;
	fl->isp_num = (reg_val & CSIC_ISP_NUM_MASK) >> CSIC_ISP_NUM;
	fl->ncsi_num = (reg_val & CSIC_NCSI_NUM_MASK) >> CSIC_NCSI_NUM;
	fl->mcsi_num = (reg_val & CSIC_MCSI_NUM_MASK) >> CSIC_MCSI_NUM;
	fl->parser_num = (reg_val & CSIC_PARSER_NUM_MASK) >> CSIC_PARSER_NUM;
}

void csic_version_get(struct csic_version *v)
{
	unsigned int reg_val = 0;
	reg_val = vin_reg_readl(csic_top_base + CSIC_VER_REG_OFF);
	v->ver_small = (reg_val & CSIC_VER_SMALL_MASK) >> CSIC_VER_SMALL;
	v->ver_big = (reg_val & CSIC_VER_BIG_MASK) >> CSIC_VER_BIG;
}

void csic_mbus_req_mex_set(unsigned int data)
{
	vin_reg_clr_set(csic_top_base + CSIC_MBUS_REQ_MAX,
			MCSI_MEM_REQ_MAX_MASK, data << MCSI_MEM_REQ_MAX);
	vin_reg_clr_set(csic_top_base + CSIC_MBUS_REQ_MAX,
			MCSI_MEM_1_REQ_MAX_MASK, data << MCSI_MEM_1_REQ_MAX);
	vin_reg_clr_set(csic_top_base + CSIC_MBUS_REQ_MAX,
			MISP_MEM_REQ_MAX_MASK, data << MISP_MEM_REQ_MAX);
}
void csic_ptn_generation_en(unsigned int en)
{
	vin_reg_clr_set(csic_top_base + CSIC_PTN_GEN_EN_REG_OFF,
			CSIC_PTN_GEN_CYCLE_MASK, 0 << CSIC_PTN_GEN_CYCLE);
	vin_reg_clr_set(csic_top_base + CSIC_PTN_GEN_EN_REG_OFF,
			CSIC_PTN_GEN_EN_MASK, en << CSIC_PTN_GEN_EN);
	vin_reg_clr_set(csic_top_base + CSIC_PTN_GEN_EN_REG_OFF,
			CSIC_PTN_GEN_START_MASK, en << CSIC_PTN_GEN_START);
}

void csic_ptn_control(int mode, int dw, int port)
{
	vin_reg_clr_set(csic_top_base + CSIC_PTN_CTRL_REG_OFF,
			CSIC_PTN_CLK_DIV_MASK, 0 << CSIC_PTN_CLK_DIV);
	vin_reg_clr_set(csic_top_base + CSIC_PTN_CTRL_REG_OFF,
			CSIC_PTN_MODE_MASK, mode << CSIC_PTN_MODE);
	vin_reg_clr_set(csic_top_base + CSIC_PTN_CTRL_REG_OFF,
			CSIC_PTN_DATA_WIDTH_MASK, dw << CSIC_PTN_DATA_WIDTH);
	vin_reg_clr_set(csic_top_base + CSIC_PTN_CTRL_REG_OFF,
			CSIC_PTN_PORT_SEL_MASK, port << CSIC_PTN_PORT_SEL);
}

void csic_ptn_length(unsigned int len)
{
	vin_reg_writel(csic_top_base + CSIC_PTN_LEN_REG_OFF, len);
}

void csic_ptn_addr(unsigned long dma_addr)
{
	vin_reg_writel(csic_top_base + CSIC_PTN_ADDR_REG_OFF, dma_addr >> 2);
}

void csic_ptn_size(unsigned int w, unsigned int h)
{
	vin_reg_clr_set(csic_top_base + CSIC_PTN_SIZE_REG_OFF,
			CSIC_PTN_WIDTH_MASK, w << CSIC_PTN_WIDTH);
	vin_reg_clr_set(csic_top_base + CSIC_PTN_SIZE_REG_OFF,
			CSIC_PTN_HEIGHT_MASK, h << CSIC_PTN_HEIGHT);
}

/*
 * functions about ccu register
 */
int csic_ccu_set_base_addr(unsigned long addr)
{
	csic_ccu_base = addr;

	return 0;
}

void csic_ccu_clk_gating_enable(void)
{
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_MODE_REG_OFF,
			CSIC_CCU_CLK_GATING_DISABLE_MASK, 0 << CSIC_CCU_CLK_GATING_DISABLE);
}

void csic_ccu_clk_gating_disable(void)
{
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_MODE_REG_OFF,
			CSIC_CCU_CLK_GATING_DISABLE_MASK, 1 << CSIC_CCU_CLK_GATING_DISABLE);
}

void csic_ccu_mcsi_clk_mode(unsigned int mode)
{
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_MODE_REG_OFF,
			CSIC_MCSI_CLK_MODE_MASK, mode << CSIC_MCSI_CLK_MODE);
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_MODE_REG_OFF,
			CSIC_MCSI_POST_CLK_MODE_MASK, mode << CSIC_MCSI_POST_CLK_MODE);
}

void csic_ccu_mcsi_combo_clk_en(unsigned int sel, unsigned int en)
{
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_PARSER_CLK_EN_REG_OFF,
			CSIC_MCSI_COMBO0_CLK_EN_MASK << sel, en << (CSIC_MCSI_COMBO0_CLK_EN + sel));
}

void csic_ccu_mcsi_mipi_clk_en(unsigned int sel, unsigned int en)
{
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_PARSER_CLK_EN_REG_OFF,
			CSIC_MCSI_MIPI0_CLK_EN_MASK << sel, en << (CSIC_MCSI_MIPI0_CLK_EN + sel));
}

void csic_ccu_mcsi_parser_clk_en(unsigned int sel, unsigned int en)
{
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_PARSER_CLK_EN_REG_OFF,
			CSIC_MCSI_PARSER0_CLK_EN_MASK << sel, en << (CSIC_MCSI_PARSER0_CLK_EN + sel));
}

void csic_ccu_misp_isp_clk_en(unsigned int sel, unsigned int en)
{
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_ISP_CLK_EN_REG_OFF,
			CSIC_MISP0_CLK_EN_MASK << sel, en << (CSIC_MISP0_CLK_EN + sel));
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_ISP_CLK_EN_REG_OFF,
			CSIC_MISP0_BRIDGE_CLK_EN_MASK << sel, en << (CSIC_MISP0_BRIDGE_CLK_EN + sel));
}

void csic_ccu_mcsi_post_clk_enable(unsigned int sel)
{
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_POST0_CLK_EN_REG_OFF + sel*4,
			CSIC_MCSI_POST0_CLK_EN_MASK, 1 << CSIC_MCSI_POST0_CLK_EN);
}

void csic_ccu_mcsi_post_clk_disable(unsigned int sel)
{
	vin_reg_clr_set(csic_ccu_base + CSIC_CCU_POST0_CLK_EN_REG_OFF + sel*4,
			CSIC_MCSI_POST0_CLK_EN_MASK, 0 << CSIC_MCSI_POST0_CLK_EN);
}

void csic_ccu_bk_clk_en(unsigned int sel, unsigned int en)
{
	if (sel < 4)
		vin_reg_clr_set(csic_ccu_base + CSIC_CCU_POST0_CLK_EN_REG_OFF,
			CSIC_MCSI_BK0_CLK_EN_MASK << sel, en << (CSIC_MCSI_BK0_CLK_EN + sel));
	else
		vin_reg_clr_set(csic_ccu_base + CSIC_CCU_POST0_CLK_EN_REG_OFF + 0x4,
			CSIC_MCSI_BK0_CLK_EN_MASK << (sel-4), en << (CSIC_MCSI_BK0_CLK_EN + (sel-4)));
}

void csic_ccu_vipp_clk_en(unsigned int sel, unsigned int en)
{
	if (sel < 4)
		vin_reg_clr_set(csic_ccu_base + CSIC_CCU_POST0_CLK_EN_REG_OFF,
			CSIC_MCSI_VIPP0_CLK_EN_MASK << sel, en << (CSIC_MCSI_VIPP0_CLK_EN + sel));
	else
		vin_reg_clr_set(csic_ccu_base + CSIC_CCU_POST0_CLK_EN_REG_OFF + 0x4,
			CSIC_MCSI_VIPP0_CLK_EN_MASK << (sel-4), en << (CSIC_MCSI_VIPP0_CLK_EN + (sel-4)));
}

