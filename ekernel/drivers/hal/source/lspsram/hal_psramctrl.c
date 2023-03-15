/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
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

#include "psram.h"
#include "hal_psramctrl.h"

#define DBG_PRC_DBG	0
#define DBG_PRC_ERR	0

#define PRC_LOG(flags, fmt, arg...) \
	do { \
		if (flags) \
			printf(fmt, ##arg); \
	} while (0)

#define PRC_DBG(fmt, arg...) PRC_LOG(DBG_PRC_DBG, "[PRC DBG] "fmt, ##arg)
#define PRC_ERR(fmt, arg...) PRC_LOG(DBG_PRC_ERR, "[PRC ERR] "fmt, ##arg)

#define PRC_BUG_ON(v) do {if (v) {printf("BUG at %s:%d!\n", __func__, __LINE__); while (1); }} while (0)

#define PRC_WAIT_NONE			HAL_BIT(0)
#define PRC_WAIT_TRANS_DONE             HAL_BIT(1)
#define PRC_WAIT_DATA_OVER              HAL_BIT(2)
#define PRC_WAIT_FINALIZE               HAL_BIT(7)

#define HAL_SYSLOG  printf

#define HAL_ASSERT_PARAM(exp)                                           \
	do {                                                                \
		if (!(exp)) {                                                   \
			HAL_SYSLOG("Invalid param at %s:%d\n", __func__, __LINE__); \
		}                                                               \
	} while (0)


static struct psram_ctrl *_psram_priv;

static void __psram_init_io(uint32_t p_type)
{
	uint32_t val;

	if (p_type == PSRAM_CHIP_OPI_APS32) {
		val = readl(0x4004a448);
		val &= 0x00000000;
		val |= 0x22222222;
		writel(val, 0x4004a448);

		val = readl(0x4004a44c);
		val &= 0x00000000;
		val |= 0x00022222;
		writel(val, 0x4004a44c);
	} else if (p_type == PSRAM_CHIP_OPI_APS64) {
		val = readl(0x4004a448);
		val &= 0x00000000;
		val |= 0x33333333;
		writel(val, 0x4004a448);

		val = readl(0x4004a44c);
		val &= 0x00000000;
		val |= 0x00033333;
		writel(val, 0x4004a44c);
	}

	val = readl(0x4004a45c);
	val &= 0x00000000;
	val |= 0x55555555;
	writel(val, 0x4004a45c);

	val = readl(0x4004a460);
	val &= 0x00000000;
	val |= 0x55555555;
	writel(val, 0x4004a460);

	val = readl(0x4004a464);
	val = 0x00000000;
	writel(val, 0x4004a464);

	val = readl(0x4004a468);
	val = 0x00000000;
	writel(val, 0x4004a468);

	val = readl(0x4004a448);
	PRC_DBG("1:gpio func:0x%08x\n", val);
	val = readl(0x4004a44c);
	PRC_DBG("2:gpio func:0x%08x\n", val);
}

static void __psram_deinit_io(uint32_t p_type)
{
#if 0
#ifdef FPGA_PLATFORM
	normal_gpio_set_t fpga_psram_gpio[13] = {
		{3, 0, 0xf, 0, 1, 1, {0} },
		{3, 1, 0xf, 0, 1, 1, {0} },
		{3, 2, 0xf, 0, 1, 1, {0} },
		{3, 3, 0xf, 0, 1, 1, {0} },
		{3, 4, 0xf, 0, 1, 1, {0} },
		{3, 5, 0xf, 0, 1, 1, {0} },
		{3, 6, 0xf, 0, 1, 1, {0} },
		{3, 7, 0xf, 0, 1, 1, {0} },
		{3, 8, 0xf, 0, 1, 1, {0} },
		{3, 9, 0xf, 0, 1, 1, {0} },
		{3, 10, 0xf, 0, 1, 1, {0} },
		{3, 11, 0xf, 0, 1, 1, {0} },
		{3, 12, 0xf, 0, 1, 1, {0} },
	};
	boot_set_gpio(fpga_psram_gpio, 13, 1);
	PRC_DBG("%s,%d disabled gpio for type:%d\n", __func__, __LINE__, p_type);
#endif
#endif
}

static void __psram_ccmu_init(void)
{
	uint32_t value;
	__IO uint32_t gat = BUS_CLK_GATING_CTRL0;
	__IO uint32_t rst = MOD_RESET_CTRL0;
	//gating
	value = readl(gat);
	value |= HAL_BIT(PSRAM_CTRL_CLK_GATING);
	writel(value, gat);
	//rst
	value = readl(rst);
	value |= HAL_BIT(PSRAM_CTRL_RST);
	writel(value, rst);
}

static void __psram_ccmu_deinit(void)
{
	uint32_t value;
	__IO uint32_t gat = BUS_CLK_GATING_CTRL0;
	__IO uint32_t rst = MOD_RESET_CTRL0;
	//gating
	value = readl(gat);
	value &= ~(1 << PSRAM_CTRL_CLK_GATING);
	writel(value, gat);
	//rst
	value = readl(rst);
	value &= ~(1 << PSRAM_CTRL_RST);
	writel(value, rst);
}

static void __psram_ccmu_en_mclk(void)
{
	uint32_t value;
	__IO uint32_t mclk = PSRAM_CLKCFG;

	value = readl(mclk);
	value |= HAL_BIT(PSRAM_CLK_ENABLE);
	writel(value, mclk);
}

static void __psram_ccmu_dis_mclk(void)
{
	uint32_t value;
	__IO uint32_t mclk = PSRAM_CLKCFG;

	value = readl(mclk);
	value &= ~(1 << PSRAM_CLK_ENABLE);
	writel(value, mclk);
}

static void __psram_pll_select(int pll)
{
	int value;
	__IO uint32_t pll_sel = PSRAMC_DEV_CLK_CTRL;

	if (pll == PSRAM_DPLL1) {
		value = readl(pll_sel);
		value &= ~HAL_BIT(CKPLL_PSRAM_SEL);
		writel(value, pll_sel);
	} else if (pll == PSRAM_DPLL3) {
		value = readl(pll_sel);
		value |= HAL_BIT(CKPLL_PSRAM_SEL);
		writel(value, pll_sel);
	}
}

static void __psram_pll_freq_set(int pll, uint32_t freq)
{
	__IO uint32_t pll1_freq = PSRAMC_DPLL1_CTRL;
	__IO uint32_t pll3_freq = PSRAMC_DPLL3_CTRL;
	uint32_t clk = 0, value = 0;

	if (freq == 192000000)
		clk = 0x301;
	else if (freq == 96000000)
		clk = 0x181;
	else if (freq == 200000000)
		clk = 0x281;

	if (pll == PSRAM_DPLL1) {
		value = readl(pll1_freq);
		value &= ~(0xfff);
		value |= clk;
		value |= HAL_BIT(31);
		writel(value, pll1_freq);
	} else if (pll == PSRAM_DPLL3) {
		value = readl(pll3_freq);
		value &= ~(0xfff);
		value |= clk;
		value |= HAL_BIT(31);
		writel(value, pll3_freq);
	}
}

static void __psram_pll_set(int pll, int enable, uint32_t div_m)
{
	uint32_t value;
	__IO uint32_t pll1 = PSRAMC_DPLL1_OUT_CTRL;
	__IO uint32_t pll3 = PSRAMC_DPLL3_OUT_CTRL;


	__psram_pll_select(pll);

	if (enable) {
		if (pll == PSRAM_DPLL1) {
			value = readl(pll1);
			value |= (div_m << CK1_PSRAM_DIV);
			writel(value, pll1);

			value = readl(pll1);
			value |= HAL_BIT(CK1_PSRAM_EN);
			writel(value, pll1);
		} else if (pll == PSRAM_DPLL3) {
			value = readl(pll3);
			value &= ~(0x7 << CK3_PSRAM_DIV);
			value |= (div_m << CK3_PSRAM_DIV);
			writel(value, pll3);

			value = readl(pll3);
			value |= HAL_BIT(CK3_PSRAM_EN);
			writel(value, pll3);
		}
	} else {
		if (pll == PSRAM_DPLL1) {
			value = readl(pll1);
			value &= ~HAL_BIT(CK1_PSRAM_EN);
			writel(value, pll1);
		} else if (pll == PSRAM_DPLL3) {
			value = readl(pll3);
			value &= ~HAL_BIT(CK3_PSRAM_EN);
			writel(value, pll3);
		}
	}
}

static void __psram_config_ccmu(int clk_src, uint32_t div_n)
{
	uint32_t value;
	__IO uint32_t clk_addr = PSRAM_CLKCFG;

	value = readl(clk_addr);
	value |= HAL_BIT(PSRAM_CLK_SRC_SEL);
	writel(value, clk_addr);

	value = readl(clk_addr);
	value &= ~(0x3);
	value |= div_n;
	writel(value, clk_addr);
}

/*
 * div_m = 5, div_n = 1
 * dpll1 = 1920MHz
 * ck1_lspsram = dpll1 / div_m = 384MHz
 * lpsram_clk = ck1_lspsram / div_n = 384MHz
 * lpsramctrl_opi_clk = lpsram_clk / div2 = 192MHz
 *
 * __psram_pll_set(PSRAM_DPLL1, 1, ?);
 * ckn_lspsram = dpll1 / div_m
 * n = 1 or 3
 * div_m =
 * 000 = 8,
 * 001 = 7,
 * 010 = 6,
 * 011 = 5
 * 1XX = 4,
 *
 * __psram_config_ccmu(1, ?);
 * clk_src 0:HFCLK 1:CK_LSPSRAM
 * ck_lspsram / div_n = lpsram_clk
 * div_n=
 * 0x00 is /1
 * 0x01 is /2
 * 0x10 is /4
 * 0x11 is /8
 * note:!!!
 * lpsram real clk is lpsramctrl_opi_clk
 * lpsramctrl_opi_clk = lpsram_clk / 2
 */

int32_t HAL_PsramCtrl_ConfigCCMU(uint32_t clk)
{
	__psram_ccmu_dis_mclk();
#ifndef CONFIG_PSRAM_200M
	__psram_pll_set(PSRAM_DPLL1, 1, 0x3);
	__psram_pll_freq_set(PSRAM_DPLL1, clk);
#else
	__psram_pll_set(PSRAM_DPLL3, 1, 0x4);
	__psram_pll_freq_set(PSRAM_DPLL3, clk);
#endif
	__psram_ccmu_init();
	__psram_config_ccmu(1, 0x0);
	__psram_ccmu_en_mclk();

	return 0;
}

void HAL_PLL_FREQ_SET(int pll, uint32_t freq)
{
	__psram_ccmu_dis_mclk();
	__psram_pll_freq_set(pll, freq);
	__psram_ccmu_en_mclk();
}

static uint32_t PSRAM_CTRL_PollHandler(struct psram_ctrl *ctrl_para)
{
	struct psram_ctrl *ctrl;
	struct psram_request *mrq;
	uint32_t status_int, inte;
	uint32_t i;
	uint32_t len = 0;
	uint32_t data_len;
	uint32_t flags;

	if (ctrl_para)
		ctrl = ctrl_para;
	else
		ctrl = _psram_priv;

	mrq = ctrl->mrq;

	flags = mrq->data.flags;

	PRC_BUG_ON(!ctrl);
	if (mrq)
		len = mrq->data.blksz * mrq->data.blocks;

	status_int = PSRAM_CTRL->INT_STA_REG;
	inte = PSRAM_CTRL->INT_EN_REG;
	ctrl->status_int = status_int;
	ctrl->inte = inte;
	PRC_DBG("%d %s status_int:0x%x enable_int:0x%x\n", __LINE__, __func__, status_int, inte);

	if (PSRAMC_RD_TOUT_INT_EN & status_int) {
			PSRAM_CTRL->INT_STA_REG = 0;
			PSRAM_CTRL->INT_STA_REG = PSRAMC_RD_TOUT_INT_EN;
	}

	if ((PSRAMC_ISR_TRANS_END & status_int) && (PSRAMC_IER_TRANS_ENB & inte)) {
		if ((ctrl->wait == PRC_WAIT_DATA_OVER) && (flags & PSRAM_DATA_READ_MASK)) {
		//if (flags & PSRAM_DATA_READ_MASK) {
			PRC_BUG_ON(!mrq || !mrq->data.buff);
			PRC_BUG_ON(!len);
			data_len = PSRAM_CTRL->FIFO_STA_REG & 0xFF;
			for (i = 0; i < data_len; i++) {
				*(mrq->data.buff + ctrl->rd_buf_idx) = readb(&PSRAM_CTRL->S_RDATA_REG);
				ctrl->rd_buf_idx++;
			}

			if (ctrl->rd_buf_idx > (len - 1)) {
				ctrl->rd_buf_idx = 0;
				len = 0;
				if (ctrl->wait == PRC_WAIT_DATA_OVER)
					ctrl->wait = PRC_WAIT_FINALIZE;
				PSRAM_CTRL->INT_EN_REG = 0;
			}
		}
		if (ctrl->wait == PRC_WAIT_TRANS_DONE || ctrl->wait == PRC_WAIT_DATA_OVER)
			ctrl->wait = PRC_WAIT_FINALIZE;
		PSRAM_CTRL->INT_EN_REG = 0;
		PSRAM_CTRL->INT_STA_REG = PSRAMC_ISR_TRANS_END;
	}

	//---Write page by irq---fifo full
	if (ctrl->wait == PRC_WAIT_DATA_OVER && (PSRAMC_WR_FIFO_FULL_FLAG & status_int) &&
		(PSRAMC_WR_FIFO_FULL_EN & inte) && (flags & PSRAM_DATA_WRITE_MASK)) {
	//if ((PSRAMC_WR_FIFO_FULL_FLAG & status_int) && (PSRAMC_WR_FIFO_FULL_EN & inte) && (flags & PSRAM_DATA_WRITE_MASK)) {
		PRC_BUG_ON(!mrq || !mrq->data.buff);
		PRC_BUG_ON(!len);
		ctrl->Psram_WR_FULL = 1;
		PSRAM_CTRL->INT_STA_REG = PSRAMC_WR_FIFO_FULL_FLAG;
	}

	//---Write page by irq---fifo empty
	if (ctrl->wait == PRC_WAIT_DATA_OVER && (PSRAMC_WR_FIFO_EMP_FLAG & status_int) &&
		(PSRAMC_WR_FIFO_EMP_EN & inte) && (flags & PSRAM_DATA_WRITE_MASK)) {
	//if ((PSRAMC_WR_FIFO_EMP_FLAG & status_int) && (PSRAMC_WR_FIFO_EMP_EN & inte) && (flags & PSRAM_DATA_WRITE_MASK)) {
		PRC_BUG_ON(!mrq || !mrq->data.buff);
		PRC_BUG_ON(!len);
		ctrl->Psram_WR_FULL = 0;
		PSRAM_CTRL->INT_STA_REG = PSRAMC_WR_FIFO_EMP_FLAG;
	}

	//---Read page by irq
	if (ctrl->wait == PRC_WAIT_DATA_OVER && (PSRAMC_RD_FIFO_FULL_FLAG & status_int) &&
		(PSRAMC_RD_FIFO_FULL_EN & inte) && (flags & PSRAM_DATA_READ_MASK)) {
	//if ((PSRAMC_RD_FIFO_FULL_FLAG & status_int) && (PSRAMC_RD_FIFO_FULL_EN & inte) && (flags & PSRAM_DATA_READ_MASK)) {
		PRC_BUG_ON(!mrq || !mrq->data.buff);
		PRC_BUG_ON(!len);
		data_len = PSRAM_CTRL->FIFO_STA_REG & 0xff;
		for (i = 0; i < data_len; i++) {
			PRC_BUG_ON(!mrq || !mrq->data.buff);
			*(mrq->data.buff + ctrl->rd_buf_idx) = readb(&PSRAM_CTRL->S_RDATA_REG);
			ctrl->rd_buf_idx++;
		}

		if (ctrl->rd_buf_idx > (len - 1)) {
			ctrl->rd_buf_idx = 0;
			if (ctrl->wait == PRC_WAIT_DATA_OVER) {
				ctrl->wait = PRC_WAIT_FINALIZE;
			}
			PSRAM_CTRL->INT_EN_REG = 0;
			//clear all bit
			PSRAM_CTRL->INT_STA_REG = PSRAMC_RD_FIFO_FULL_FLAG;
		}
	}
	/*if ((DMA_WR_CROSS_OP_FLAG & status_int) && (DMA_WR_CROSS_OP_FLAG & inte)) {
		PSRAM_CTRL->INT_STA_REG = DMA_WR_CROSS_OP_FLAG;
	} */

	return 0;
}

static int32_t __psram_ctrl_request_done(struct psram_ctrl *ctrl)
{
	int32_t ret = 0;
	struct psram_request *mrq = ctrl->mrq;


	if (ctrl->wait != PRC_WAIT_FINALIZE) {
		PRC_ERR("Psram wait transfer time out! %x %x\n", ctrl->status_int, ctrl->inte);
		return -1;
	}
	if (mrq && mrq->cmd.resp) {
		uint32_t idx = 0;
		uint32_t len = mrq->data.blksz * mrq->data.blocks;

		while (len--) {
			if ((!(PSRAM_CTRL->FIFO_STA_REG & 0xff)) &&
					(mrq->data.flags & PSRAM_DATA_READ_MASK)) {
				PRC_ERR("fifo is null\n");
				break;
			}
			mrq->cmd.resp[idx++] = readb(&PSRAM_CTRL->S_RDATA_REG);
		}
	}
	PSRAM_CTRL->PSRAM_COM_CFG &= ~PSRAMC_MR_REG_ADDR_EN;

	return ret;

}

#define PSRAM_WR_CHECK_LEN      (12)
#define PSRAM_MAGIC_WORD_DATA1  (0x00ff00ff)
#define PSRAM_MAGIC_WORD_DATA2  (0x5aa55aa5)
#define PSRAM_MAGIC_WORD_DATA3  (0xa55aa55a)

static int32_t __psram_wr_check(uint32_t addr)
{
	uint32_t *ptr = (uint32_t *)(uintptr_t)addr;
	for (int i = 0; i < PSRAM_WR_CHECK_LEN; i += 3) {
		ptr[i] = PSRAM_MAGIC_WORD_DATA1;
		ptr[i + 1] = PSRAM_MAGIC_WORD_DATA2;
		ptr[i + 2] = PSRAM_MAGIC_WORD_DATA3;
	}
	memset((uint8_t *)0x1500000, 0, 128);
	for (int i = 0; i < PSRAM_WR_CHECK_LEN; i += 3) {
		if (ptr[i] != PSRAM_MAGIC_WORD_DATA1
			|| ptr[i + 1] != PSRAM_MAGIC_WORD_DATA2
			|| ptr[i + 2] != PSRAM_MAGIC_WORD_DATA3) {
			return -1;
		}
	}
	return 0;
}

int32_t HAL_PsramCtrl_DQS_Delay_Cal_Policy(struct psram_ctrl *ctrl)
{
	uint8_t dqsCal;
	uint8_t minCal = 0;
	uint8_t maxCal = 0;

	//dcacheWT_idx = HAL_Dcache_Enable_WriteThrough(PSRAM_START_ADDR, rounddown2(PSRAM_END_ADDR, 16));
	minCal = 0;
	maxCal = 0;
	for (int j = 0; j < 64; j++) {
		HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_DQS_IN_DLY_CFG,\
				PSRAMC_OVERWR_CAL_MASK | PSRAMC_OVERWR_CAL,\
				PSRAMC_OVERWR_CAL_VAL(j) | PSRAMC_OVERWR_CAL);
		hal_udelay(10);
		if (!(__psram_wr_check(PSRAM_START_ADDR)\
				|| __psram_wr_check(PSRAM_END_ADDR + 1 -\
				PSRAM_WR_CHECK_LEN * sizeof(uint32_t)))) {
			if (minCal == 0) {
				minCal = j;
			}
			maxCal = j;
		} else {
			if ((maxCal != 0) && (minCal != maxCal)) {
				break;
			} else if (minCal != 0) {
				minCal = 0;
				maxCal = 0;
			}
		}
		PRC_DBG("---j:%d\n", j);
	}
	PRC_DBG("maxCal:%x minCal:%x\n", maxCal, minCal);
	if (maxCal - minCal < 2) {
		PRC_ERR("%s: Can not find correct dqs\n", __func__);
		goto out;
	}
	dqsCal = (minCal + maxCal) / 2;
	PRC_DBG("%s: minCal=%d, maxCal=%d, set cal=%d\n", __func__, minCal, maxCal, (minCal + maxCal) / 2);
	HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_DQS_IN_DLY_CFG, PSRAMC_OVERWR_CAL_MASK | PSRAMC_OVERWR_CAL,
			PSRAMC_OVERWR_CAL_VAL(dqsCal) | PSRAMC_OVERWR_CAL);

	hal_udelay(10);
out:
	//HAL_Dcache_Disable_WriteThrough(dcacheWT_idx);
	return -1;
}

int32_t HAL_PsramCtrl_DQS_Delay_Calibration(void)
{
	uint8_t dqsCal;

	HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_DQS_IN_DLY_CFG, HAL_BIT(0), 0x01);

	while (1) {
		dqsCal = HAL_GET_BIT(PSRAM_CTRL->PSRAM_DQS_IN_DLY_CFG, HAL_BIT(0));
		if (dqsCal == 0)
			break;
		else
			hal_udelay(10);
	}
	dqsCal = HAL_GET_BIT_VAL(PSRAM_CTRL->PSRAM_DQS_IN_DLY_CFG, 0x4, 0x3f);

	if (dqsCal >= 63) {
		PRC_ERR("cal_value is too large, can not calibrate\n");
		return -1;
	} else {
		dqsCal = dqsCal / 2;
		HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_DQS_IN_DLY_CFG, PSRAMC_OVERWR_CAL_MASK | PSRAMC_OVERWR_CAL,
				PSRAMC_OVERWR_CAL_VAL(dqsCal) | PSRAMC_OVERWR_CAL);
	}

	return 0;
}

static void __psram_ctrl_send_cmd(struct psram_ctrl *ctrl, struct psram_command *cmd)
{
	PSRAM_CTRL->START_SEND_REG = 0x01;
}

void HAL_PsramCtrl_MaxCE_LowCyc(struct psram_ctrl *ctrl, uint32_t clk)
{
	uint32_t ce_cyc = 0;
	ce_cyc = 4 * (clk / 1000 / 1000) - 32;
	ce_cyc = clk < 8000000 ? 1 : ce_cyc;

	HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_COM_CFG, PSRAMC_MAX_CEN_LOW_CYC_MASK,
			PSRAMC_MAX_CEN_LOW_CYC_NUM(ce_cyc));
}

typedef struct {
	uint32_t cbus_read_op;
	uint32_t cbus_write_op;
	uint32_t common_cfg;
	uint32_t p_common_cfg;
} PsramCtrl_Delay;

#if 0
static void __psram_ctrl_bus_delay(struct psram_ctrl *ctrl, PsramCtrl_Delay *delay)
{
	PSRAM_CTRL->C_READ_CFG = delay->cbus_read_op;
	PSRAM_CTRL->C_WRITE_CFG = delay->cbus_write_op;
	PSRAM_CTRL->PSRAM_COM_CFG = delay->common_cfg;
	PSRAM_CTRL->MEM_COM_CFG = delay->p_common_cfg;
}
#endif

int32_t HAL_PsramCtrl_Request(struct psram_ctrl *ctrl, struct psram_request *mrq, uint8_t standby_flag)
{
	int32_t ret;
	int32_t len;
	//unsigned long iflags;
	uint32_t wait = PRC_WAIT_NONE;

	len = mrq->data.blksz * mrq->data.blocks;

	PRC_DBG("%s,%d cmd:0x%x addr:0x%x len:%d flag:0x%x bc:0x%x \n",
			__func__, __LINE__, mrq->cmd.opcode, mrq->cmd.addr,
			len, mrq->data.flags, ctrl->busconfig);
	//iflags = HAL_EnterCriticalSection();
	//iflags = arch_irq_save();
	PSRAM_CTRL->S_RW_CFG = mrq->cmd.opcode << PSRAMC_S_RW_CFG_RW_COM_SEND_SHIFT |
				ctrl->busconfig;

	//sbus read
	if (mrq->data.flags & PSRAM_DATA_READ_MASK) {
		PSRAM_CTRL->S_ADDR_CFG = mrq->cmd.addr;
		PSRAM_CTRL->S_RD_NUM = len;
		PSRAM_CTRL->PSRAM_COM_CFG |= PSRAMC_MR_REG_ADDR_EN; //sbus rw should after bit14 set 1
	}

	//sbus write
	if (mrq->data.flags & PSRAM_DATA_WRITE_MASK) {
		uint32_t value;
		uint8_t *buf;

		PSRAM_CTRL->S_ADDR_CFG = mrq->cmd.addr;
		PSRAM_CTRL->S_WR_NUM = len;
		if (mrq->cmd.opcode != Global_Reaet)
			PSRAM_CTRL->PSRAM_COM_CFG |= PSRAMC_MR_REG_ADDR_EN; //sbus rw should after bit14 set 1
		buf = mrq->data.buff;
		while (len > 0) {
			if (mrq->data.flags & PSRAM_DATA_WRITE_BYTE) {
				value = readb(buf);
				writeb(value, &PSRAM_CTRL->S_WDATA_REG);
				len--;
				buf++;
			} else if (mrq->data.flags & PSRAM_DATA_WRITE_SHORT) {
				value = readw(buf);
				writew(value, &PSRAM_CTRL->S_WDATA_REG);
				len -= 2;
				buf += 2;
			} else if (mrq->data.flags & PSRAM_DATA_WRITE_WORD) {
				value = readl(buf);
				writel(value, &PSRAM_CTRL->S_WDATA_REG);
				len -= 4;
				buf += 4;
			}
		}
	}

	HAL_MODIFY_REG(PSRAM_CTRL->INT_EN_REG, PSRAMC_IER_TRANS_ENB_MASK,
			PSRAMC_IER_TRANS_ENB | PSRAMC_RD_TOUT_INT_EN);
	//HAL_ExitCriticalSection(iflags);
	//arch_irq_restore(iflags);

	ctrl->mrq = mrq;
	wait = PRC_WAIT_TRANS_DONE;
	ctrl->wait = wait;
	__psram_ctrl_send_cmd(ctrl, &mrq->cmd);

	for (uint16_t tryCnt = 0; tryCnt < 0x3FFF; tryCnt++) {
		if (standby_flag)
			PSRAM_CTRL_PollHandler(ctrl);
		else
			PSRAM_CTRL_PollHandler(NULL);

		if (ctrl->wait == PRC_WAIT_FINALIZE) {
			break;
		}
	}

	ret = __psram_ctrl_request_done(ctrl);
	PSRAM_CTRL->PSRAM_COM_CFG &= ~PSRAMC_MR_REG_ADDR_EN;
	ctrl->wait = PRC_WAIT_NONE;
	ctrl->mrq = NULL;
	if (ret)
		PRC_ERR("%s,%d err:%d!!\n", __func__, __LINE__, ret);
	return ret;
}

void HAL_PsramCtrl_IDbusCfg(struct psram_ctrl *ctrl, uint32_t write, uint32_t opcfg)
{
	HAL_CLR_BIT(PSRAM_CTRL->MEM_COM_CFG, PSRAMC_CBUS_RW_EN);
	if (write) {
		PSRAM_CTRL->C_WRITE_CFG = opcfg;
	} else {
		PSRAM_CTRL->C_READ_CFG = opcfg;
	}
	HAL_SET_BIT(PSRAM_CTRL->MEM_COM_CFG, PSRAMC_CBUS_RW_EN);
}

void HAL_PsramCtrl_Set_CBUS_WR_LATENCY(struct psram_ctrl *ctrl, uint32_t lat)
{
	HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_LAT_CFG, PSRAMC_CBUS_WR_LC_MASK, lat);
	PRC_DBG("%s set CBUS write latency:0x%x\n", __func__, lat);
}

void HAL_PsramCtrl_Set_SBUS_WR_LATENCY(struct psram_ctrl *ctrl, uint32_t lat)
{
	HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_LAT_CFG, PSRAMC_SBUS_WR_LC_MASK, lat);
	PRC_DBG("%s set SBUS write latency:0x%x\n", __func__, lat);
}

uint32_t HAL_PsramCtrl_Set_BusWidth(struct psram_ctrl *ctrl, uint32_t width)
{
	uint32_t back_width;

	HAL_ASSERT_PARAM(ctrl != NULL);

	back_width = ctrl->busconfig;
	ctrl->busconfig = width;

	return back_width;
}

void HAL_PsramCtrl_CacheCfg(struct psram_ctrl *ctrl, uint32_t cbus_wsize_bus)
{
	if (cbus_wsize_bus) {
		HAL_SET_BIT(PSRAM_CTRL->CACHE_CFG, PSRAMC_CBUS_WR_SEL_BUS);
	} else {
		HAL_CLR_BIT(PSRAM_CTRL->CACHE_CFG, PSRAMC_CBUS_WR_SEL_BUS);
	}
}

void HAL_PsramCtrl_DMACrossEnable(uint32_t en)
{
	uint32_t rval;

	if (en)
		rval = PSRAMC_IDBUS_DMA_EN;
	else
		rval = 0;
	PRC_DBG("%s set IDBUS DMA enable:0x%x\n", __func__, PSRAM_CTRL->C_READ_CFG);
	HAL_MODIFY_REG(PSRAM_CTRL->C_READ_CFG, PSRAMC_IDBUS_DMA_EN, rval);
	PRC_DBG("%s set IDBUS DMA enable:0x%x\n", __func__, PSRAM_CTRL->C_READ_CFG);
	//HAL_MODIFY_REG(PSRAM_CTRL->C_WRITE_CFG, PSRAMC_IDBUS_DMA_EN, rval);
	//PRC_DBG("%s set CBUS DMA enable:0x%x\n", __func__, PSRAM_CTRL->C_WRITE_CFG);
}

void HAL_PsramCtrl_Set_Address_Field(struct psram_ctrl *ctrl, uint32_t id,
		 uint32_t startaddr, uint32_t endaddr, uint32_t bias_addr)
{
	HAL_ASSERT_PARAM(id < 4);

	PSRAM_CTRL->PSRAM_ADDR[id].END_ADDR = PSRAMC_END_POS(endaddr);
	PSRAM_CTRL->PSRAM_ADDR[id].START_ADDR = PSRAMC_START_POS(startaddr);
#ifndef CFG_PSRAM_DISABLE_CACHE
	PSRAM_CTRL->PSRAM_ADDR[id].BIAS_ADDR = bias_addr | PSRAMC_ADDR_BIAS_EN;
#else
	PSRAM_CTRL->PSRAM_ADDR[id].BIAS_ADDR = bias_addr;
#endif
}

/*
 * @brief Initialize Psram controller.
 * @param cfg:
 *	@arg cfg->freq: Psram working frequency.
 * @retval HAL_Status: The status of driver.
 */
HAL_Status HAL_PsramCtrl_Init(struct psram_ctrl *ctrl, const PSRAMCtrl_InitParam *cfg)
{
	PsramCtrl_Delay delay;

	HAL_ASSERT_PARAM(ctrl != NULL);
	HAL_ASSERT_PARAM(cfg != NULL);

	//HAL_SemaphoreInit(&ctrl->lock, 0, 1);
	//HAL_SemaphoreInit(&ctrl->dmaSem, 0, 1);

	/* enable gpio */
	__psram_init_io(ctrl->p_type);

	/* enable ccmu */
	//HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_PSRAM_CTRL);
	//HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_PSRAM_CTRL);
#ifndef CONFIG_PSRAM_200M
	/* shoult set ccmu to 96M before set ccmu to 192M.
	 * read MR register will more stable
	 */
	HAL_PsramCtrl_ConfigCCMU(96000000);
#else
	int val = readl(0x4004c4ac);
	val = val | HAL_BIT(0) | HAL_BIT(16);
	writel(val, 0x4004c4ac);
	HAL_PsramCtrl_ConfigCCMU(200000000);
#endif

	HAL_PsramCtrl_DQS_Delay_Calibration();

	PSRAM_CTRL->S_RW_CFG = 0;
	//PSRAM_CTRL->S_DUMMY_DATA_H = 0;
	//PSRAM_CTRL->S_DUMMY_DATA_L = 0;
	//PSRAM_CTRL->S_IO_SW_WAIT_TIME = PSRAMC_SBUS_DUM_WAIT(0) | PSRAMC_SBUS_ADR_WAIT(0) | PSRAMC_SBUS_CMD_WAIT(0);

	//PSRAM_CTRL->OPI_COM_CFG = PSRAM_SPI_MODE0 | PSRAM_SPI_MSB | PSRAM_SPI_CS_L |
	//				PSRAM_SPI_RDWAIT_Cycle(ctrl->rdata_w) | PSRAM_DMA_CROSS_OP_DIS;
	PSRAM_CTRL->OPI_COM_CFG = 0x1;
	//PSRAM_CTRL->C_IO_SW_WAIT_TIME = PSRAMC_CBUS_DUM_WAIT(0) | PSRAMC_CBUS_ADR_WAIT(0) | PSRAMC_CBUS_CMD_WAIT(0);
	PSRAM_CTRL->FIFO_TRIG_LEV = PSRAMC_FIFO_WR_FULL_TRIG(0x38) | PSRAMC_FIFO_WR_EMPT_TRIG(0x0F) |
					PSRAMC_FIFO_RD_FULL_TRIG(0x38) | PSRAMC_FIFO_RD_EMPT_TRIG(0x0F);

	//PSRAM_CTRL->C_READ_CFG = 0x3000000;
	//PSRAM_CTRL->C_WRITE_CFG = 0x2000000;

	switch (cfg->p_type) {
	case PSRAM_CHIP_OPI_APS32:
		PSRAM_CTRL->PSRAM_COM_CFG = 0xc1c02158;
		PSRAM_CTRL->MEM_COM_CFG = 0x60;

		PSRAM_CTRL->PSRAM_MISC_CFG = 0x702;
#ifndef CFG_PSRAM_DISABLE_CACHE
		PSRAM_CTRL->CACHE_CFG = 0x00009009;
#endif
		PSRAM_CTRL->CACHE_CFG = 0x0000a00a;

#define MF12 1

#ifdef FPGA_PLATFORM
#ifdef MF12
		PSRAM_CTRL->PSRAM_TIM_CFG = 0x1121211; //bitfile aw1883_system_lpsram_u2_0707_19_psram.rbf
#endif
#endif

		break;
	case PSRAM_CHIP_OPI_APS64:
		PSRAM_CTRL->PSRAM_COM_CFG = 0xc1c02001;
		PSRAM_CTRL->MEM_COM_CFG = 0x60;
		PSRAM_CTRL->PSRAM_MISC_CFG = 0x706;
		PSRAM_CTRL->CACHE_CFG = 0x0000b00b;
		PSRAM_CTRL->PSRAM_LAT_CFG = 0x200700;
#ifdef FPGA_PLATFORM
		/* PSRAM_CTRL->PSRAM_TIM_CFG = 0x1221311; //bitfile aw1883_system_lpsram_u2_0707_19_psram.rbf */
#endif
		break;
	default:
		break;
	}

	switch (cfg->p_type) {
	case PSRAM_CHIP_OPI_APS32:
		delay.cbus_read_op = PSRAMC_CBUS_CMD_8BIT | PSRAMC_CBUS_ADDR_8BIT | PSRAMC_CBUS_DUMY_8BIT |
			PSRAMC_CBUS_DUMY_WID(0) | PSRAMC_CBUS_DATA_8BIT;
		delay.cbus_read_op = 0x3000000;
		delay.cbus_write_op = PSRAMC_CBUS_WR_CMD(0x80) | PSRAMC_CBUS_WR_OP_CMD_8BIT |
			PSRAMC_CBUS_WR_OP_ADDR_8BIT |
			PSRAMC_CBUS_WR_OP_DUMY_8BIT | PSRAMC_CBUS_WR_OP_DUMY_NUM(0) |
			PSRAMC_CBUS_WR_OP_DATA_8BIT;
		delay.cbus_write_op = 0x2000000;
		delay.common_cfg = PSRAMC_MAX_READ_LATENCY(0x0CU) | PSRAMC_MAX_CEN_LOW_CYC_NUM(0x1C0U) | PSRAMC_CLK_STOP_CE_LOW |
			PSRAMC_MIN_WR_CLC_2 | PSRAMC_DDR_MODE_EN | PSRAMC_CLK_OUTPUT_HLD |
			PSRAMC_WR_NEED_DQS | PSRAMC_WR_AF_DQS_DUMMY | PSRAMC_WR_AF_DM_DUMMY |
			PSRAMC_CEDIS_CLK_VALID;
		if (cfg->freq > 66000000) {
			delay.common_cfg |= PSRAMC_COM_DQS_READ_WAIT(0x2U);
		} else {
			delay.common_cfg |= PSRAMC_COM_DQS_READ_WAIT(0x1U);
		}
		//delay.common_cfg = 0xc1c02158;
		delay.p_common_cfg = PSRAMC_IOX_DEF_OUTPUT_MODE(1, PSRAMC_IOX_DEF_OUTPUTZ) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(2, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(3, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(4, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(5, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(6, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(7, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAM_SELECT | PSRAMC_ADDR_SIZE_24BIT | PSRAMC_CBUS_RW_EN;
		//delay.p_common_cfg = 0xe0;
		//HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_TIM_CFG, PSRAMC_CS_OUTP_DHCYC_MASK, PSRAMC_CS_OUTP_DHCYC(2));
		HAL_PsramCtrl_Set_BusWidth(ctrl, PSRAMC_SBUS_CMD_SEND_8BIT | PSRAMC_SBUS_DATA_GW_8BIT);
		//HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_TIM_CFG, PSRAMC_DQS_OUTP_DHCYC_MASK, PSRAMC_DQS_OUTP_DHCYC(2));
#ifdef FPGA_PLATFORM
		if (cfg->freq >= 24 * 1000 * 1000) {
			HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_TIM_CFG, PSRAMC_DQS_OUTP_DHCYC_MASK, PSRAMC_DQS_OUTP_DHCYC(3));
		}
#endif
		break;
	case PSRAM_CHIP_OPI_APS64 :
		delay.cbus_read_op = PSRAMC_CBUS_CMD_8BIT | PSRAMC_CBUS_ADDR_8BIT | PSRAMC_CBUS_DUMY_8BIT |
			PSRAMC_CBUS_DUMY_WID(0) | PSRAMC_CBUS_DATA_8BIT; //0x444004
		delay.cbus_write_op = PSRAMC_CBUS_WR_CMD(0x80) | PSRAMC_CBUS_WR_OP_CMD_8BIT |
			PSRAMC_CBUS_WR_OP_ADDR_8BIT |
			PSRAMC_CBUS_WR_OP_DUMY_8BIT | PSRAMC_CBUS_WR_OP_DUMY_NUM(0) |
			PSRAMC_CBUS_WR_OP_DATA_8BIT;//0x80444004
		delay.common_cfg = PSRAMC_MAX_READ_LATENCY(0xCU) | PSRAMC_MAX_CEN_LOW_CYC_NUM(0xA0U) |
			PSRAMC_COM_DQS_READ_WAIT(0x2U) | PSRAMC_CMD_HLD_THCYC | PSRAMC_DDR_MODE_EN;
		delay.p_common_cfg = PSRAMC_IOX_DEF_OUTPUT_MODE(1, PSRAMC_IOX_DEF_OUTPUTZ) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(2, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(3, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(4, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(5, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(6, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAMC_IOX_DEF_OUTPUT_MODE(7, PSRAMC_IOX_DEF_OUTPUT1) |
			PSRAM_SELECT | PSRAMC_CBUS_RW_EN | PSRAMC_ADDR_SIZE_32BIT;
		HAL_PsramCtrl_Set_BusWidth(ctrl, PSRAMC_SBUS_CMD_SEND_8BIT | PSRAMC_SBUS_DATA_GW_8BIT);
		/*
		HAL_MODIFY_REG(PSRAM_CTRL->PSRAM_TIM_CFG, (PSRAMC_CS_OUTP_DHCYC_MASK | PSRAMC_DQS_OUTP_DHCYC_MASK),
			(PSRAMC_CS_OUTP_DHCYC(2) | PSRAMC_DQS_OUTP_DHCYC(2)));
		*/
		break;
	default:
		PRC_ERR("%s,%d not support chip:%x\n", __func__, __LINE__, cfg->p_type);
		goto fail;
	}
	//__psram_ctrl_bus_delay(ctrl, &delay);

	PSRAM_CTRL->INT_EN_REG = 0; /* clear all irq */
	PSRAM_CTRL->INT_STA_REG = 0xffffffff;
	//HAL_NVIC_ConfigExtIRQ(PSRAMC_IRQn, PSRAM_CTRL_IRQHandler, NVIC_PERIPH_PRIO_DEFAULT);

	PRC_DBG("%s busconfig:0x%x\n", __func__, ctrl->busconfig);

	return HAL_OK;

fail:
	//HAL_CCM_PSRAMC_DisableMClock();
	__psram_pll_set(1, 0, 0);
	__psram_ccmu_dis_mclk();

	return HAL_ERROR;
}

/*
 * @brief Deinitialize Psram controller.
 * @param None
 * @retval HAL_Status: The status of driver.
 */
HAL_Status HAL_PsramCtrl_Deinit(struct psram_ctrl *ctrl)
{
	__psram_deinit_io(ctrl->p_type);

	//HAL_NVIC_DisableIRQ(PSRAMC_IRQn);
	//HAL_NVIC_SetIRQHandler(PSRAMC_IRQn, 0);
	//HAL_CCM_PSRAMC_DisableMClock();
	__psram_pll_set(1, 0, 0);
	__psram_ccmu_dis_mclk();
	//HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_PSRAM_CTRL);
	//HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_PSRAM_CTRL);
	__psram_ccmu_deinit();
	//HAL_SemaphoreDeinit(&ctrl->dmaSem);
	//HAL_SemaphoreDeinit(&ctrl->lock);

	return HAL_OK;
}

/*
 * @brief Open psram controller SBUS.
 * @note At the same time, it will disable XIP and suspend schedule.
 * @param None
 * @retval HAL_Status: The status of driver.
 */
struct psram_ctrl *HAL_PsramCtrl_Open(uint32_t id)
{
	_psram_priv->ref++;

	return _psram_priv;
}

/*
 * @brief Close psram controller SBUS.
 * @param None
 * @retval HAL_Status: The status of driver.
 */
HAL_Status HAL_PsramCtrl_Close(struct psram_ctrl *ctrl)
{
	ctrl->ref--;

	return HAL_OK;
}

struct psram_ctrl *HAL_PsramCtrl_Create(uint32_t id, const PSRAMCtrl_InitParam *cfg)
{
	struct psram_ctrl *ctrl;

	PRC_DBG("%s\n", __func__);
	ctrl = malloc(sizeof(struct psram_ctrl));
	if (ctrl == NULL) {
		PRC_ERR("%s no mem!\n", __func__);
	} else {
		memset(ctrl, 0, sizeof(struct psram_ctrl));
		ctrl->freq = cfg->freq;
		ctrl->p_type = cfg->p_type;
		ctrl->rdata_w = cfg->rdata_w;
		_psram_priv = ctrl;
		PRC_DBG("ctrl->freq:%d\n", ctrl->freq);
	}
	PRC_DBG("%s ::@%x\n", __func__, ctrl);

	return ctrl;
}

HAL_Status HAL_PsramCtrl_Destory(struct psram_ctrl *ctrl)
{
	if (ctrl == NULL) {
		PRC_ERR("ctrl %p", ctrl);
	} else {
		_psram_priv = NULL;
		free(ctrl);
	}
	PRC_DBG("%s @%p\n", __func__, ctrl);

	return HAL_OK;
}
