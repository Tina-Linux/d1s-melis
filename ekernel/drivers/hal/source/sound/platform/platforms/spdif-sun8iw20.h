/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
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
#ifndef	__SUN8IW20_SPDIF_H_
#define	__SUN8IW20_SPDIF_H_

#define	SUNXI_SPDIF_MEMBASE (0x02036000)

/*add this to surport the spdif receive IEC-61937 data */
#define CONFIG_SND_SUNXI_SPDIF_RX_IEC61937

/*------------------------ CLK CONFIG FOR NORMAL ---------------------------*/
#define SUNXI_SPDIF_CLK_PLL_AUDIO	CLK_PLL_AUDIO0
#define SUNXI_SPDIF_CLK_SPDIF		CLK_SPDIF_TX
#define SUNXI_SPDIF_CLK_BUS		CLK_BUS_SPDIF
#define SUNXI_SPDIF_CLK_RST		RST_BUS_SPDIF
#define SUNXI_SPDIF_CLK_PLL_AUDIO1	CLK_PLL_AUDIO1
#define SUNXI_SPDIF_CLK_PLL_AUDIO1_DIV	CLK_PLL_AUDIO1_DIV5
#define SUNXI_SPDIF_CLK_SPDIF_RX	CLK_SPDIF_RX

/*------------------SPDIF EXP register definition--------------------*/
#define	SUNXI_SPDIF_EXP_CTL	0x40
#define	SUNXI_SPDIF_EXP_ISTA	0x44
#define	SUNXI_SPDIF_EXP_INFO0	0x48
#define	SUNXI_SPDIF_EXP_INFO1	0x4C
#define	SUNXI_SPDIF_EXP_DBG0	0x50
#define	SUNXI_SPDIF_EXP_DBG1	0x54
#define	SUNXI_SPDIF_EXP_VER	0x58

/* SUNXI_SPDIF_EXP_CTL register */
#define INSET_DET_NUM		0
#define INSET_DET_EN		8
#define SYNCW_BIT_EN		9
#define DATA_TYPE_BIT_EN	10
#define DATA_LEG_BIT_EN		11
#define AUDIO_DATA_BIT_EN	12
#define RX_MODE			13
#define RX_MODE_MAN		14
#define UNIT_SEL		15
#define RPOTBF_NUM		16
#define BURST_DATA_OUT_SEL	30

/* SUNXI_SPDIF_EXP_ISTA register */
#define INSET_INT		0
#define PAPB_CAP_INT		1
#define PCPD_CAP_INT		2
#define RPDB_ERR_INT		3
#define PC_DTYOE_CH_INT		4
#define PC_ERR_FLAG_INT		5
#define PC_BIT_CH_INT		6
#define PC_PAUSE_STOP_INT	7
#define PD_CHAN_INT		8
#define INSET_INT_EN		16
#define PAPB_CAP_INT_EN		17
#define PCPD_CAP_INT_EN		18
#define RPDB_ERR_INT_EN		19
#define PC_DTYOE_CH_INT_EN	20
#define PC_ERR_FLAG_INT_EN	21
#define PC_BIT_CH_INT_EN	22
#define PC_PAUSE_STOP_INT_EN	23
#define PD_CHAN_INT_EN		24

/* SUNXI_SPDIF_EXP_INFO0 register */
#define PD_DATA_INFO		0
#define PC_DATA_INFO		16

/* SUNXI_SPDIF_EXP_INFO1 register */
#define SAMPLE_RATE_VAL		0
#define RPOTBF_VAL		16

/* SUNXI_SPDIF_EXP_DBG0 register */
#define RE_DATA_COUNT_VAL	0
#define DATA_CAP_STA_MACHE	16

/* SUNXI_SPDIF_EXP_DBG1 register */
#define SAMPLE_RATE_COUNT	0
#define RPOTBF_COUNT		16

/* SUNXI_SPDIF_EXP_VER register */
#define MOD_VER			0

/*------------------------ PIN CONFIG FOR NORMAL ---------------------------*/



/*------------------------ PIN CONFIG FOR FPGA VERIFY -----------------------*/
spdif_gpio_t g_spdif_gpio = {
//	.clk	= {GPIOB(12), 3},
	.out	= {GPIOB(6), 2},
	.in	= {GPIOB(7), 2},
};

/*------------------------ CLK CONFIG FOR SUN8IW20 ---------------------------*/
struct sunxi_spdif_clk {
	struct reset_control *rstclk;

	hal_clk_t pllclk;
	hal_clk_t moduleclk;
	hal_clk_t busclk;
	hal_clk_t pllclk1;
	hal_clk_t pllclk1_div;
	hal_clk_t moduleclk_rx;
};


static inline int snd_sunxi_spdif_clk_enable(struct sunxi_spdif_clk *clk)
{
	int ret;

	ret = hal_reset_control_deassert(clk->rstclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_deassert rstclk failed.\n");
		goto err_spdif_pllclk_enable;
	}

	ret = hal_clock_enable(clk->busclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_enable busclk failed.\n");
		goto err_spdif_pllclk_enable;
	}
	ret = hal_clock_enable(clk->pllclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_enable pllclk failed.\n");
		goto err_spdif_pllclk_enable;
	}
	ret = hal_clock_enable(clk->moduleclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_enable moduleclk failed.\n");
		goto err_spdif_moduleclk_enable;
	}
#ifdef CONFIG_SND_SUNXI_SPDIF_RX_IEC61937
	ret = hal_clock_enable(clk->pllclk1);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_enable pllclk1 failed.\n");
		goto err_spdif_pllclk_enable;
	}
	ret = hal_clock_enable(clk->pllclk1_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_enable pllclk1 div failed.\n");
		goto err_spdif_pllclk_enable;
	}
	ret = hal_clock_enable(clk->moduleclk_rx);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_enable moduleclk rx failed.\n");
		goto err_spdif_moduleclk_enable;
	}
#endif

	return HAL_CLK_STATUS_OK;

err_spdif_moduleclk_enable:
	hal_clock_disable(clk->pllclk);
err_spdif_pllclk_enable:

	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_spdif_clk_disable(struct sunxi_spdif_clk *clk)
{
	hal_clock_disable(clk->busclk);
	hal_clock_disable(clk->moduleclk);
	hal_clock_disable(clk->pllclk);
#ifdef CONFIG_SND_SUNXI_SPDIF_RX_IEC61937
	hal_clock_disable(clk->moduleclk_rx);
	hal_clock_disable(clk->pllclk1_div);
	hal_clock_disable(clk->pllclk1);
#endif
	hal_reset_control_assert(clk->rstclk);

	return;
}

static inline int snd_sunxi_spdif_clk_init(struct sunxi_spdif_clk *clk)
{
	int ret;
	hal_clk_type_t clk_type = HAL_SUNXI_CCU;
	hal_reset_type_t reset_type = HAL_SUNXI_RESET;

	clk->pllclk = hal_clock_get(clk_type, SUNXI_SPDIF_CLK_PLL_AUDIO);
	clk->moduleclk = hal_clock_get(clk_type, SUNXI_SPDIF_CLK_SPDIF);
	clk->busclk = hal_clock_get(clk_type, SUNXI_SPDIF_CLK_BUS);
	clk->rstclk = hal_reset_control_get(reset_type, SUNXI_SPDIF_CLK_RST);
#ifdef CONFIG_SND_SUNXI_SPDIF_RX_IEC61937
	clk->pllclk1 = hal_clock_get(clk_type, SUNXI_SPDIF_CLK_PLL_AUDIO1);
	clk->pllclk1_div = hal_clock_get(clk_type, SUNXI_SPDIF_CLK_PLL_AUDIO1_DIV);
	clk->moduleclk_rx = hal_clock_get(clk_type, SUNXI_SPDIF_CLK_SPDIF_RX);
#endif

	ret = hal_clk_set_parent(clk->moduleclk, clk->pllclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("clk clk_set_parent failed.\n");
		goto err_spdif_moduleclk_set_parent;
	}
#ifdef CONFIG_SND_SUNXI_SPDIF_RX_IEC61937
	ret = hal_clk_set_parent(clk->moduleclk_rx, clk->pllclk1_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("clk clk_set_parent failed.\n");
		goto err_spdif_moduleclk_set_parent;
	}
#endif

	ret = snd_sunxi_spdif_clk_enable(clk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif snd_sunxi_spdif_clk_enable failed.\n");
		goto err_clk_enable;
	}

	return HAL_CLK_STATUS_OK;

err_clk_enable:
err_spdif_moduleclk_set_parent:
	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_spdif_clk_exit(struct sunxi_spdif_clk *clk)
{
	snd_sunxi_spdif_clk_disable(clk);

	hal_clock_put(clk->busclk);
	hal_clock_put(clk->moduleclk);
	hal_clock_put(clk->pllclk);
#ifdef CONFIG_SND_SUNXI_SPDIF_RX_IEC61937
	hal_clock_put(clk->moduleclk_rx);
	hal_clock_put(clk->pllclk1_div);
	hal_clock_put(clk->pllclk1);
#endif
	hal_reset_control_put(clk->rstclk);

	return;
}

static inline int snd_sunxi_spdif_clk_set_rate(struct sunxi_spdif_clk *clk, int stream,
					       unsigned int freq_in, unsigned int freq_out)
{
	int ret;

	(void)stream;
	(void)freq_in;

	ret = hal_clk_set_rate(clk->pllclk, freq_out);
	if (ret < 0) {
		snd_err("set pllclk %u failed\n", freq_out);
		return HAL_CLK_STATUS_ERROR;
	}

	return HAL_CLK_STATUS_OK;
}

#endif /* __SUN8IW20_SPDIF_H_ */
